#include "ui_manager.h"
#include <regex>
#include <sstream>

UIManager::UIManager(const std::string& ui_file) {
    root = UIParser::parse(ui_file);
    state["count"] = 0;
    state["score"] = 0;
}

void UIManager::render() {
    if (root->type == "ui") {
        for (const auto& child : root->children) {
            if (child->type == "win") {
                parse_window_config(child);
                ImGui::SetNextWindowPos(ImVec2(0, 0));
                ImGui::SetNextWindowSize(ImVec2(window_config.width, window_config.height));
                ImGuiWindowFlags flags = 0;
                if (!window_config.resizable) flags |= ImGuiWindowFlags_NoResize;
                if (!window_config.movable) flags |= ImGuiWindowFlags_NoMove;
                
                ImGui::Begin(window_config.title.c_str(), nullptr, flags);
                for (const auto& element : child->children) {
                    render_element(element);
                }
                ImGui::End();
            }
        }
    }
}

void UIManager::render_element(const std::shared_ptr<UIElement>& element) {
    if (element->type == "win") {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        ImGui::Begin(element->text.c_str(), nullptr, 
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        for (const auto& child : element->children) {
            render_element(child);
        }
        
        ImGui::End();
    }
    else if (element->type == "txt") {
        if (!element->position.empty()) {
            handle_position(element->position);
        }
        ImGui::Text("%s", format_text(element->text).c_str());
    }
    else if (element->type == "btn") {
        if (!element->position.empty()) {
            handle_position(element->position);
        }
        if (ImGui::Button(element->text.c_str())) {
            handle_action(element->action);
        }
    }
    else if (element->type == "panel") {
        if (!element->position.empty()) {
            handle_position(element->position);
        }
        for (const auto& child : element->children) {
            render_element(child);
        }
    }
    else if (element->type == "row") {
        ImGui::BeginGroup();
        for (const auto& child : element->children) {
            render_element(child);
            ImGui::SameLine();
        }
        ImGui::EndGroup();
    }
}

void UIManager::handle_position(const std::string& position) {
    if (position.find("center") != std::string::npos) {
        // Extract y value if present: center(y:100)
        size_t pos = position.find("y:");
        if (pos != std::string::npos) {
            std::string y_val = position.substr(pos + 2);
            y_val.erase(std::remove_if(y_val.begin(), y_val.end(), 
                [](char c) { return c == ')' || std::isspace(c); }), y_val.end());
            if (!y_val.empty()) {
                ImGui::SetCursorPosY(std::stof(y_val));
            }
        }
        
        // Center horizontally
        float window_width = ImGui::GetWindowSize().x;
        float text_width = ImGui::CalcTextSize("Centered Text").x;
        ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
    }
    else if (position.find("below") != std::string::npos) {
        // Extract offset: below(20)
        size_t start = position.find('(');
        if (start != std::string::npos) {
            std::string offset = position.substr(start + 1);
            offset.erase(std::remove_if(offset.begin(), offset.end(),
                [](char c) { return c == ')' || std::isspace(c); }), offset.end());
            if (!offset.empty()) {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + std::stof(offset));
            }
        }
    }
}

void UIManager::handle_action(const std::string& action) {
    // Handle increment actions (count++, score++, etc)
    if (action.length() > 2 && action.substr(action.length() - 2) == "++") {
        std::string var = action.substr(0, action.length() - 2);
        state[var]++;
    }
}

std::string UIManager::format_text(const std::string& text) {
    std::string result = text;
    std::regex pattern("\\{(\\w+)\\}");
    
    std::string::const_iterator start = result.begin();
    std::string::const_iterator end = result.end();
    std::smatch match;
    std::string formatted;
    
    while (std::regex_search(start, end, match, pattern)) {
        formatted += std::string(start, start + match.position());
        std::string key = match[1].str();
        formatted += std::to_string(state[key]);
        start += match.position() + match.length();
    }
    formatted += std::string(start, end);
    return formatted;
}

void UIManager::set_state(const std::string& key, int value) {
    state[key] = value;
}

int UIManager::get_state(const std::string& key) const {
    return state.at(key);
}

void UIManager::parse_window_config(const std::shared_ptr<UIElement>& win_element) {
    window_config.title = win_element->text;
    
    if (win_element->properties.count("size")) {
        auto& size = win_element->properties["size"];
        // Parse array like [400, 300]
        if (size.length() > 4 && size[0] == '[') {
            size_t comma = size.find(',');
            if (comma != std::string::npos) {
                std::string width = size.substr(1, comma - 1);
                std::string height = size.substr(comma + 1);
                // Remove spaces and brackets
                width.erase(std::remove_if(width.begin(), width.end(), ::isspace), width.end());
                height.erase(std::remove_if(height.begin(), height.end(), ::isspace), height.end());
                if (height.back() == ']') height.pop_back();
                
                window_config.width = std::stof(width);
                window_config.height = std::stof(height);
            }
        }
    }
    if (win_element->properties.count("background")) {
        auto& bg = win_element->properties["background"];
        // Parse array like [0.2, 0.2, 0.2]
        if (bg.length() > 6 && bg[0] == '[') {
            std::string str = bg.substr(1, bg.length() - 2);
            size_t pos1 = str.find(',');
            size_t pos2 = str.find(',', pos1 + 1);
            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                window_config.background.r = std::stof(str.substr(0, pos1));
                window_config.background.g = std::stof(str.substr(pos1 + 1, pos2 - pos1 - 1));
                window_config.background.b = std::stof(str.substr(pos2 + 1));
            }
        }
    }
}
 