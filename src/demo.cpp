
#include <cstdlib>
#define PROPERTY_MAP_IMPLEMENTATION
#include "propmap.hpp"

int main(int argc, char *argv[])
{
    property_map some_entity;
    some_entity.add_property<float>("fuel_capacity",12.0);
    some_entity.add_property<float>("fuel_remaining",1.4);

    if(some_entity.has_property("fuel_capacity"))
    {
        float fuelcapacity=some_entity.get_property<float>("fuel_capacity");
        float fuel = some_entity.get_property<float>("fuel_remaining");
        printf("capacity=%f, remaining=%f\n",fuelcapacity,fuel);
    }
    else
    {
        printf("property not found\n");
    }
    return EXIT_SUCCESS;
}