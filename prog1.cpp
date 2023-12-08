#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

void readTextFiles(const std::string& directoryPath) {
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.path().extension() == ".txt" || entry.path().extension() == ".doc") {
            std::ifstream file(entry.path());
            if (file.is_open()) {
                std::string line;
                std::cout << "Contents of file " << entry.path().filename() << ":\n";
                while (std::getline(file, line)) {
                    std::cout << line << '\n';
                }
                file.close();
            }
            else {
                std::cerr << "Unable to open file: " << entry.path().filename() << "\n";
            }
        }
    }
}

int main() {
    std::string directoryPath;
    std::cout << "Enter the path to the directory containing .txt and .doc files: ";
    std::cin >> directoryPath;

    readTextFiles(directoryPath);

    return 0;
}