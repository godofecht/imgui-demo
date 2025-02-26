#include "ui_parser.h"
#include <fstream>
#include <sstream>

std::shared_ptr<UIElement> UIParser::parse(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    std::string_view view(content);
    
    auto root = std::make_shared<UIElement>();
    parse_element(root, view);
    return root;
}

void UIParser::skip_whitespace(std::string_view& content) {
    while (!content.empty() && std::isspace(content.front())) {
        content.remove_prefix(1);
    }
}

std::string UIParser::parse_string(std::string_view& content) {
    if (content.empty() || content.front() != '"') {
        return "";
    }
    
    content.remove_prefix(1); // Skip opening quote
    std::string result;
    
    while (!content.empty() && content.front() != '"') {
        result += content.front();
        content.remove_prefix(1);
    }
    
    if (!content.empty()) {
        content.remove_prefix(1); // Skip closing quote
    }
    
    return result;
}

std::string UIParser::parse_identifier(std::string_view& content) {
    std::string result;
    while (!content.empty() && (std::isalnum(content.front()) || content.front() == '_')) {
        result += content.front();
        content.remove_prefix(1);
    }
    return result;
}

std::string UIParser::parse_position(std::string_view& content) {
    if (content.empty() || content.front() != '@') {
        return "";
    }
    
    content.remove_prefix(1); // Skip @
    skip_whitespace(content);
    
    std::string result;
    while (!content.empty() && !std::isspace(content.front()) && content.front() != '{' && content.front() != '>') {
        result += content.front();
        content.remove_prefix(1);
    }
    
    // Handle position parameters in parentheses
    if (!content.empty() && content.front() == '(') {
        result += content.front();
        content.remove_prefix(1);
        
        while (!content.empty() && content.front() != ')') {
            result += content.front();
            content.remove_prefix(1);
        }
        
        if (!content.empty()) {
            result += content.front(); // Add closing parenthesis
            content.remove_prefix(1);
        }
    }
    
    return result;
}

std::string UIParser::parse_action(std::string_view& content) {
    if (content.empty() || content.front() != '>' || content[1] != '>') {
        return "";
    }
    
    content.remove_prefix(2); // Skip >>
    skip_whitespace(content);
    
    std::string result;
    while (!content.empty() && !std::isspace(content.front()) && content.front() != '{') {
        result += content.front();
        content.remove_prefix(1);
    }
    
    return result;
}

void UIParser::parse_element(std::shared_ptr<UIElement>& element, std::string_view& content) {
    skip_whitespace(content);
    
    element->type = parse_identifier(content);
    skip_whitespace(content);
    
    if (!content.empty() && content.front() == '"') {
        element->text = parse_string(content);
        skip_whitespace(content);
    }
    
    // Parse properties before position/action
    while (!content.empty() && content.front() != '@' && content.front() != '>' && content.front() != '{') {
        std::string key = parse_identifier(content);
        skip_whitespace(content);
        
        if (!content.empty() && content.front() == ':') {
            content.remove_prefix(1);
            skip_whitespace(content);
            std::string value;
            
            // Handle array values [x, y, z]
            if (!content.empty() && content.front() == '[') {
                value += content.front();
                content.remove_prefix(1);
                while (!content.empty() && content.front() != ']') {
                    value += content.front();
                    content.remove_prefix(1);
                }
                if (!content.empty()) {
                    value += content.front(); // Add closing bracket
                    content.remove_prefix(1);
                }
            } else {
                // Handle simple values
                while (!content.empty() && !std::isspace(content.front()) && content.front() != '{') {
                    value += content.front();
                    content.remove_prefix(1);
                }
            }
            
            element->properties[key] = value;
            skip_whitespace(content);
        }
    }
    
    if (!content.empty() && content.front() == '@') {
        element->position = parse_position(content);
        skip_whitespace(content);
    }
    
    if (content.length() >= 2 && content.front() == '>' && content[1] == '>') {
        element->action = parse_action(content);
        skip_whitespace(content);
    }
    
    if (!content.empty() && content.front() == '{') {
        content.remove_prefix(1);
        skip_whitespace(content);
        
        while (!content.empty() && content.front() != '}') {
            auto child = std::make_shared<UIElement>();
            parse_element(child, content);
            element->children.push_back(child);
            skip_whitespace(content);
        }
        
        if (!content.empty()) {
            content.remove_prefix(1); // Skip closing brace
        }
    }
} 