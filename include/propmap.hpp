/**
 * Copyright (c) 2004 - 2024 David Rowbotham.
 *
 */

#ifndef PROPERTY_MAP_API
#define PROPERTY_MAP_API

// remove comment to enable thread safety.
//#define PROPERTY_MAP_THREAD_SAFE (1)


#include <memory>
#include <map>
#include <string>
#ifdef PROPERTY_MAP_THREAD_SAFE
#include <mutex>
#endif
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
    property_map(){};
    ~property_map(){};

    template <typename T>
    T &get_property(const std::string &name)
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::unique_lock<std::mutex> lock(m);
#endif
        if (properties.contains(name))
        {
            property<T> *pointer = (dynamic_cast<property<T> *>(properties[name].get()));
            if (pointer)
            {
                return pointer->val;
            }
            else
            {
                throw "invalid property";
            }
        }
        else
        {
            throw "non-existant property";
        }
    }
    template <typename T>
    const bool set_property(const std::string &name, const T &val)
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::unique_lock<std::mutex> lock(m);
#endif
        bool result{false};
        if (properties.contains(name))
        {
            auto success = properties.emplace(std::pair<std::string, std::unique_ptr<property_base>>(name, std::move(std::make_unique(property < T >> (val)))));
            result = success.second;
        }
        else
        {
            property<T> *pointer = (dynamic_cast<property<T>>(properties[name].get()));
            if (pointer)
            {
                pointer->val = val;
                result = true;
            }
        }
        return result;
    }
    const bool has_property(const std::string &name);
    template <typename T>
    const bool add_property(const std::string &name, const T &val)
    {
#ifdef PROPERTY_MAP_THREAD_SAFE
        std::unique_lock<std::mutex> lock(m);
#endif
        if (properties.contains(name))
        {
            return false;
        }
        auto success = properties.emplace(std::pair<std::string, std::unique_ptr<property_base>>(name, std::move(std::make_unique<property<T>>(val))));
        return success.second;
    }
    void remove_property(const std::string &name);
    void clear_properties();

private:
    std::map<const std::string, std::unique_ptr<property_base>> properties;
#ifdef PROPERTY_MAP_THREAD_SAFE
    std::mutex m;
#endif
};
#endif

#ifdef PROPERTY_MAP_IMPLEMENTATION
void test_property_map()
{
    property_map some_entity;
    some_entity.add_property<float>("fuel_capacity", 12.0);
    some_entity.add_property<float>("fuel_remaining", 1.4);

    if (some_entity.has_property("fuel_capacity"))
    {
        float fuelcapacity = some_entity.get_property<float>("fuel_capacity");
        float fuel = some_entity.get_property<float>("fuel_remaining");
        printf("capacity=%f, remaining=%f\n", fuelcapacity, fuel);
    }
    else
    {
        printf("property not found\n");
    }
}
const bool property_map::has_property(const std::string &name)
{
    #ifdef PROPERTY_MAP_THREAD_SAFE
    std::unique_lock<std::mutex> lock(m);
    #endif
    return properties.contains(name);
}
void property_map::remove_property(const std::string &name)
{
    #ifdef PROPERTY_MAP_THREAD_SAFE
    std::unique_lock<std::mutex> lock(m);
    #endif
    if (properties.contains(name))
    {
        properties.erase(name);
    }
}
void property_map::clear_properties()
{
    #ifdef PROPERTY_MAP_THREAD_SAFE
    std::unique_lock<std::mutex> lock(m);
    #endif
    properties.clear();
}
#endif