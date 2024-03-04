#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>

std::vector<std::pair<std::string, float>> read_instrument_init_data_from_file(const std::string& file_path) {
    std::vector<std::pair<std::string, float>> instruments_info;

    std::ifstream input_file(file_path);

    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return instruments_info;
    }

    std::string line;
    while (std::getline(input_file, line)) {
        std::istringstream iss(line);

        std::string symbol;
        float value;

        if (iss >> symbol >> value) {
            instruments_info.push_back(std::make_pair(symbol, value));
        } else {
            std::cerr << "Error parsing line: " << line << std::endl;
        }
    }

    input_file.close();

    return instruments_info;
}
