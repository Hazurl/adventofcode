#include <fstream>
#include <iostream>
#include <filesystem>
#include <iomanip>

std::filesystem::path formatted_directory = "./formatted_inputs/";
std::filesystem::path inputs_directory = "./inputs/";

int main() {
    for(std::filesystem::path input_path : std::filesystem::directory_iterator(inputs_directory)) {
        auto output_path = formatted_directory / input_path.filename();

        if (! std::filesystem::exists(output_path)) {
            std::ifstream input (input_path);
            std::ofstream output (output_path);

            output << "#include <array>\n";
            output << "static constexpr std::array<long long, xxxxxx> input {\n";

            long long size = 0;
            for(std::string line; std::getline(input, line); size++) {
                output << '\t' << line << ",\n";
            }

            output << "};\n";

            output.seekp(56);
            output << std::setw(6) << std::setfill(' ') << size;
        }
    }
}