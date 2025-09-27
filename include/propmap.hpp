/**
 * Copyright (c) 2004 - 2024 David Rowbotham.
 *
 */

#ifndef PROPERTY_MAP_API
#define PROPERTY_MAP_API

// remove comment to enable thread safety.
//#define PROPERTY_MAP_THREAD_SAFE (1)


#include <memory>
#include <unordered_map>
#include <string>
#include <string_view>
#include <stdexcept>
#include <optional>
#include <functional>
#ifdef PROPERTY_MAP_THREAD_SAFE
#include <shared_mutex>
#endif

/**
 * Custom exception for property map errors
 */
class property_error : public std::exception
{
private:
    std::string message;

public:
    explicit property_error(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

struct property_base
{
    property_base() {}
    virtual ~property_base(){};
};

template <typename T>
struct property : public property_base
{
    property(const T &val)
        : val(val) {}
    ~property() {}
    T val;
};

class property_map
{
public:
    using iterator = std::unordered_map<std::string, std::unique_ptr<property_base>>::iterator;
    using const_iterator = std::unordered_map<std::string, std::unique_ptr<property_base>>::const_iterator;

    property_map() = default;
    ~property_map() = default;
    
    // Move constructor and assignment
    property_map(property_map&&) = default;
    property_map& operator=(property_map&&) = default;
    
    // Disable copy constructor and assignment (due to unique_ptr)
    property_map(const property_map&) = delete;
    property_map& operator=(const property_map&) = delete;

    /**
     * Get a property by name with type checking
     * Throws property_error if property doesn't exist or type mismatch
     */
    template <typename T>
    T& get_property(std::string_view name)
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::shared_lock<std::shared_mutex> lock(m);
#endif
        std::string key{name}; // Convert string_view to string for map key
        auto it = properties.find(key);
        if (it != properties.end())
        {
            property<T>* pointer = dynamic_cast<property<T>*>(it->second.get());
            if (pointer)
            {
                return pointer->val;
            }
            else
            {
                throw property_error("Invalid property type for: " + key);
            }
        }
        else
        {
            throw property_error("Non-existent property: " + key);
        }
    }

    /**
     * Const version of get_property
     */
    template <typename T>
    const T& get_property(std::string_view name) const
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::shared_lock<std::shared_mutex> lock(m);
#endif
        std::string key{name};
        auto it = properties.find(key);
        if (it != properties.end())
        {
            const property<T>* pointer = dynamic_cast<const property<T>*>(it->second.get());
            if (pointer)
            {
                return pointer->val;
            }
            else
            {
                throw property_error("Invalid property type for: " + key);
            }
        }
        else
        {
            throw property_error("Non-existent property: " + key);
        }
    }

    /**
     * Safe property access that returns std::optional
     */
    template <typename T>
    std::optional<std::reference_wrapper<T>> try_get_property(std::string_view name) noexcept
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::shared_lock<std::shared_mutex> lock(m);
#endif
        std::string key{name};
        auto it = properties.find(key);
        if (it != properties.end())
        {
            property<T>* pointer = dynamic_cast<property<T>*>(it->second.get());
            if (pointer)
            {
                return std::ref(pointer->val);
            }
        }
        return std::nullopt;
    }

    /**
     * Const version of try_get_property
     */
    template <typename T>
    std::optional<std::reference_wrapper<const T>> try_get_property(std::string_view name) const noexcept
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::shared_lock<std::shared_mutex> lock(m);
#endif
        std::string key{name};
        auto it = properties.find(key);
        if (it != properties.end())
        {
            const property<T>* pointer = dynamic_cast<const property<T>*>(it->second.get());
            if (pointer)
            {
                return std::ref(pointer->val);
            }
        }
        return std::nullopt;
    }

    /**
     * Check if property exists and is of correct type
     */
    template <typename T>
    bool is_property_type(std::string_view name) const noexcept
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::shared_lock<std::shared_mutex> lock(m);
#endif
        std::string key{name};
        auto it = properties.find(key);
        if (it != properties.end())
        {
            return dynamic_cast<const property<T>*>(it->second.get()) != nullptr;
        }
        return false;
    }

    /**
     * Set or create a property
     */
    template <typename T>
    bool set_property(std::string_view name, const T& val)
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::unique_lock<std::shared_mutex> lock(m);
#endif
        std::string key{name};
        auto it = properties.find(key);
        if (it != properties.end())
        {
            // Property exists, try to update it
            property<T>* pointer = dynamic_cast<property<T>*>(it->second.get());
            if (pointer)
            {
                pointer->val = val;
                return true;
            }
            return false; // Type mismatch
        }
        else
        {
            // Property doesn't exist, create it
            auto success = properties.emplace(key, std::make_unique<property<T>>(val));
            return success.second;
        }
    }

    /**
     * Set property with move semantics
     */
    template <typename T>
    bool set_property(std::string_view name, T&& val)
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::unique_lock<std::shared_mutex> lock(m);
#endif
        std::string key{name};
        auto it = properties.find(key);
        if (it != properties.end())
        {
            property<T>* pointer = dynamic_cast<property<T>*>(it->second.get());
            if (pointer)
            {
                pointer->val = std::move(val);
                return true;
            }
            return false;
        }
        else
        {
            auto success = properties.emplace(key, std::make_unique<property<T>>(std::move(val)));
            return success.second;
        }
    }

    /**
     * Add property only if it doesn't exist
     */
    template <typename T>
    bool add_property(std::string_view name, const T& val)
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::unique_lock<std::shared_mutex> lock(m);
#endif
        std::string key{name};
        if (properties.find(key) != properties.end())
        {
            return false;
        }
        auto success = properties.emplace(key, std::make_unique<property<T>>(val));
        return success.second;
    }

    /**
     * Add property with move semantics
     */
    template <typename T>
    bool add_property(std::string_view name, T&& val)
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::unique_lock<std::shared_mutex> lock(m);
#endif
        std::string key{name};
        if (properties.find(key) != properties.end())
        {
            return false;
        }
        auto success = properties.emplace(key, std::make_unique<property<T>>(std::move(val)));
        return success.second;
    }

    // Property existence and management
    bool has_property(std::string_view name) const;
    void remove_property(std::string_view name);
    void clear_properties();

    // Container interface
    size_t size() const noexcept 
    { 
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::shared_lock<std::shared_mutex> lock(m);
#endif
        return properties.size(); 
    }
    
    bool empty() const noexcept 
    { 
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::shared_lock<std::shared_mutex> lock(m);
#endif
        return properties.empty(); 
    }

    // Iterator support (note: not thread-safe, use with caution)
    iterator begin() { return properties.begin(); }
    iterator end() { return properties.end(); }
    const_iterator begin() const { return properties.begin(); }
    const_iterator end() const { return properties.end(); }
    const_iterator cbegin() const { return properties.cbegin(); }
    const_iterator cend() const { return properties.cend(); }

private:
    std::unordered_map<std::string, std::unique_ptr<property_base>> properties;
#ifdef PROPERTY_MAP_THREAD_SAFE
    mutable std::shared_mutex m;
#endif
};
#endif

#ifdef PROPERTY_MAP_IMPLEMENTATION
#include <cstdio>

void test_property_map()
{
    printf("=== Property Map Test ===\n");
    
    property_map some_entity;
    
    // Test adding properties
    printf("Adding properties...\n");
    some_entity.add_property<float>("fuel_capacity", 12.0f);
    some_entity.add_property<float>("fuel_remaining", 1.4f);
    some_entity.add_property<std::string>("name", std::string("Spaceship Alpha"));
    some_entity.add_property<int>("crew_size", 5);
    
    printf("Properties added. Total count: %zu\n", some_entity.size());
    
    // Test basic property access
    if (some_entity.has_property("fuel_capacity"))
    {
        float fuel_capacity = some_entity.get_property<float>("fuel_capacity");
        float fuel_remaining = some_entity.get_property<float>("fuel_remaining");
        printf("Fuel: %f / %f\n", fuel_remaining, fuel_capacity);
    }
    
    // Test type checking
    printf("\nType checking tests:\n");
    printf("fuel_capacity is float: %s\n", 
           some_entity.is_property_type<float>("fuel_capacity") ? "true" : "false");
    printf("fuel_capacity is int: %s\n", 
           some_entity.is_property_type<int>("fuel_capacity") ? "true" : "false");
    
    // Test safe property access
    printf("\nSafe property access tests:\n");
    auto fuel_opt = some_entity.try_get_property<float>("fuel_capacity");
    if (fuel_opt.has_value())
    {
        printf("Safe access to fuel_capacity: %f\n", fuel_opt.value().get());
    }
    
    auto invalid_opt = some_entity.try_get_property<int>("non_existent");
    if (!invalid_opt.has_value())
    {
        printf("Safe access to non_existent property: failed as expected\n");
    }
    
    // Test property updating
    printf("\nUpdating properties...\n");
    some_entity.set_property<float>("fuel_remaining", 8.5f);
    printf("Updated fuel remaining: %f\n", some_entity.get_property<float>("fuel_remaining"));
    
    // Test string property
    const auto& ship_name = some_entity.get_property<std::string>("name");
    printf("Ship name: %s\n", ship_name.c_str());
    
    // Test move semantics
    std::string new_name = "Starship Beta";
    some_entity.set_property("name", std::move(new_name));
    printf("Updated ship name: %s\n", some_entity.get_property<std::string>("name").c_str());
    
    // Test exception handling
    printf("\nException handling test:\n");
    try
    {
        some_entity.get_property<float>("non_existent_property");
    }
    catch (const property_error& e)
    {
        printf("Caught expected exception: %s\n", e.what());
    }
    
    // Test property removal
    printf("\nRemoving a property...\n");
    some_entity.remove_property("crew_size");
    printf("Properties remaining: %zu\n", some_entity.size());
    
    printf("\n=== Test completed ===\n");
}
bool property_map::has_property(std::string_view name) const
{
#ifdef PROPERTY_MAP_THREAD_SAFE
    std::shared_lock<std::shared_mutex> lock(m);
#endif
    std::string key{name};
    return properties.find(key) != properties.end();
}

void property_map::remove_property(std::string_view name)
{
#ifdef PROPERTY_MAP_THREAD_SAFE
    std::unique_lock<std::shared_mutex> lock(m);
#endif
    std::string key{name};
    auto it = properties.find(key);
    if (it != properties.end())
    {
        properties.erase(it);
    }
}

void property_map::clear_properties()
{
#ifdef PROPERTY_MAP_THREAD_SAFE
    std::unique_lock<std::shared_mutex> lock(m);
#endif
    properties.clear();
}
#endif