#include "platform/win32.cpp"
#include "core/debug/logging.cpp"

int main() {
    initialize_logging();
    LOG_INFO("Hello C++!");
}
