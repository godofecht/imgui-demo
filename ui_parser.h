#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

struct UIElement {
    std::string type;
    std::string text;
    std::vector<std::shared_ptr<UIElement>> children;
    std::unordered_map<std::string, std::string> properties;
    std::string position;
    std::string action;
};

class UIParser {
public:
    static std::shared_ptr<UIElement> parse(const std::string& filename);
private:
    static void parse_element(std::shared_ptr<UIElement>& element, std::string_view& content);
    static void skip_whitespace(std::string_view& content);
    static std::string parse_string(std::string_view& content);
    static std::string parse_identifier(std::string_view& content);
    static std::string parse_position(std::string_view& content);
    static std::string parse_action(std::string_view& content);
}; 