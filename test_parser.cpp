#include "ui_parser.h"
#include <iostream>
#include <filesystem>
#include <fstream>

void print_element(const std::shared_ptr<UIElement>& element, int indent = 0) {
    std::string ind(indent * 2, ' ');
    
    // Print type and text
    std::cout << ind << "Type: " << element->type;
    if (!element->text.empty()) {
        std::cout << ", Text: \"" << element->text << "\"";
    }
    std::cout << "\n";
    
    // Print properties
    for (const auto& [key, value] : element->properties) {
        std::cout << ind << "  " << key << ": " << value << "\n";
    }
    
    // Print position and action
    if (!element->position.empty()) {
        std::cout << ind << "  Position: " << element->position << "\n";
    }
    if (!element->action.empty()) {
        std::cout << ind << "  Action: " << element->action << "\n";
    }
    
    // Print children recursively
    for (const auto& child : element->children) {
        print_element(child, indent + 1);
    }
}

int main() {
    // Print current directory
    std::cout << "Current path: " << std::filesystem::current_path() << "\n";
    
    // Try to open the file first
    std::ifstream file("demo.ui");
    if (!file.is_open()) {
        std::cerr << "Could not open demo.ui\n";
        return 1;
    }

    // Print file contents
    std::cout << "File contents:\n";
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << "\n";
    }
    file.close();

    // Now try to parse
    std::cout << "\nParsing result:\n";
    auto root = UIParser::parse("demo.ui");
    print_element(root);
    return 0;
} 