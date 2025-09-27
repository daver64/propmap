# PropMap - Modern C++ Property Map

A high-performance, type-safe property map implementation for C++20 that provides heterogeneous key-value storage with runtime type safety and optional thread safety.

## Features

- **Type Safety**: Runtime type checking with compile-time type hints
- **Modern C++**: Uses C++20 features including `std::optional`, `string_view`, and move semantics  
- **Performance Optimized**: `std::unordered_map` backend for O(1) average lookup time
- **Exception Safety**: Custom exception types with descriptive error messages
- **Thread Safety**: Optional thread safety using `shared_mutex` for concurrent reads
- **STL Compatible**: Provides iterators, size(), empty() and other standard container interfaces
- **Memory Efficient**: Uses `std::unique_ptr` for automatic memory management
- **Safe Access**: Multiple access patterns from throwing to optional-based

## Quick Start

```cpp
#define PROPERTY_MAP_IMPLEMENTATION
#include "propmap.hpp"

property_map entity;

// Add properties of different types
entity.add_property<float>("health", 100.0f);
entity.add_property<std::string>("name", "Player");
entity.add_property<int>("level", 1);

// Access properties (throws on error)
float health = entity.get_property<float>("health");
std::string name = entity.get_property<std::string>("name");

// Safe access (returns std::optional)
auto health_opt = entity.try_get_property<float>("health");
if (health_opt.has_value()) {
    std::cout << "Health: " << health_opt.value() << std::endl;
}

// Update existing properties
entity.set_property<float>("health", 95.0f);

// Type checking
if (entity.is_property_type<float>("health")) {
    // Safe to access as float
}
```

## API Reference

### Core Methods

#### Property Management
- `add_property<T>(name, value)` - Add new property (fails if exists)
- `set_property<T>(name, value)` - Set property (creates or updates)
- `get_property<T>(name)` - Get property reference (throws on error)
- `try_get_property<T>(name)` - Safe access returning `std::optional`
- `has_property(name)` - Check if property exists
- `is_property_type<T>(name)` - Check if property exists and is of type T
- `remove_property(name)` - Remove property
- `clear_properties()` - Remove all properties

#### Container Interface
- `size()` - Number of properties
- `empty()` - Check if empty
- `begin()/end()` - Iterator support for range-based loops

### Exception Handling

The library uses a custom `property_error` exception class:

```cpp
try {
    auto value = entity.get_property<int>("nonexistent");
} catch (const property_error& e) {
    std::cout << "Error: " << e.what() << std::endl;
}
```

### Thread Safety

Enable thread safety by defining `PROPERTY_MAP_THREAD_SAFE` before including:

```cpp
#define PROPERTY_MAP_THREAD_SAFE
#define PROPERTY_MAP_IMPLEMENTATION  
#include "propmap.hpp"
```

Thread-safe operations use `std::shared_mutex` allowing multiple concurrent readers.

### Move Semantics

The library supports move semantics for efficient handling of large objects:

```cpp
std::string large_string = "...";
entity.set_property("data", std::move(large_string)); // Moves instead of copying
```

## Build Instructions

### Requirements
- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- CMake 3.0+

### Building

```bash
# Clone or download the repository
cd propmap
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the demo
cd ..
./bin/propmap
```

### Integration

PropMap is header-only. Simply include `propmap.hpp` in your project:

1. Copy `include/propmap.hpp` to your project
2. Add `#define PROPERTY_MAP_IMPLEMENTATION` before the first include
3. Optionally add `#define PROPERTY_MAP_THREAD_SAFE` for thread safety

## Use Cases

- **Game Entities**: Store heterogeneous component data  
- **Configuration Systems**: Runtime property management
- **Scripting Integration**: Bridge between C++ and scripting languages
- **Serialization**: Dynamic property storage for JSON/XML marshalling  
- **Plugin Systems**: Runtime property registration and access

## Performance Characteristics

- **Lookup**: O(1) average, O(n) worst case (hash table)
- **Insertion**: O(1) average, O(n) worst case
- **Memory**: Low overhead with `std::unique_ptr` and custom allocator support
- **Thread Safety**: Shared readers, exclusive writers when enabled

## License

Copyright (c) 2004 - 2024 David Rowbotham

See LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.
