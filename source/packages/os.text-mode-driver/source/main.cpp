#include "guid.hpp"
#include "ipc/os_message.hpp"
#include "terminal.h"

using namespace os::ipc;

extern "C" bool on_connect(guid id)
{
    // TODO: Allocate connection information.

    return false;
}

extern "C" void initialize_text_mode_driver(void *message, size_t message_size)
{
    os_message message(message, message_size);

    void *video_memory = *reinterpret_cast<void**>(message.get_value(0));
    size_t video_memory_size = *reinterpret_cast<size_t*>(message.get_value(1));

    auto standard_video_memory - 0xA0000;
    auto standard_text_mode_memory = 0xB8000;

    auto text_offset = standard_text_mode_memory - standard_video_memory;

    terminal_buffer = reinterpret_cast<uint16_t*>(reinterpret_cast<char*>(video_memory) + text_offset);

    terminal_initialize();

    terminal_writestring("Hello world!!! \n\nThe text mode driver");

}