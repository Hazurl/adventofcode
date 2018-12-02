#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <iomanip>
#include <string_view>
#include <algorithm>

std::filesystem::path formatted_directory = "./formatted_inputs/";
std::filesystem::path inputs_directory = "./inputs/";

int main(int argc, char** argv) {
    int argn = 0;

    std::filesystem::directory_iterator directories_it(inputs_directory);
    std::vector<std::filesystem::path> directories(directories_it, std::filesystem::directory_iterator{});

    std::sort(std::begin(directories), std::end(directories));

    for(std::filesystem::path input_path : directories) {
        argn++;
        auto output_path = formatted_directory / input_path.filename().concat(".inc");

        if (! std::filesystem::exists(output_path)) {
            std::cout << "Process " << output_path << " from " << input_path << " with type " << argv[argn] << '\n';
            std::ifstream input (input_path);
            std::ofstream output (output_path);

            std::string_view type(argv[argn]);

            bool is_string = type == "std::string" || type == "std::string_view" || type == "const char*";

            output << "#include <array>\n";
            output << "#include <string_view>\n";
            output << "static constexpr std::array<" << type << ", ";
            auto pos = output.tellp();
            output << "xxxxxxxxxx> input {\n";

            long long size = 0;
            for(std::string line; std::getline(input, line); size++) {
                if (is_string) {
                    output << "\t\"" << line << "\",\n";
                } else {
                    output << '\t' << line << ",\n";
                }
            }

            output << "};\n";

            output.seekp(pos);
            output << std::setw(10) << std::setfill(' ') << size;
        }
    }
}