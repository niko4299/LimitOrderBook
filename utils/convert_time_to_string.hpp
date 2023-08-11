#include <string>
#include <ctime>

inline std::string convert_time_to_string(std::time_t& timestamp) noexcept{
    char timestamp_string[80];
    std::strftime(timestamp_string, 80, "%Y-%m-%d %H:%M:%S", std::localtime(&timestamp));

    return std::string(timestamp_string);
}