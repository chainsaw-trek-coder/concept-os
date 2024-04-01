#include "guid.hpp"
#include "shared_mem.hpp"

extern "C" bool on_connect(guid id)
{
    // TODO: Allocate connection information.

    return false;
}

extern "C" void on_hardware_mem(shared_mem memory)
{

}