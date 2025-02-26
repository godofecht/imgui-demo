#pragma once
#include "ui_parser.h"
#include <imgui.h>
#include <string>
#include <unordered_map>
#include <memory>

struct WindowConfig {
    std::string title = "ImGui Demo";
    float width = 400;
    float height = 300;
    struct {
        float r = 0.2f;
        float g = 0.2f;
        float b = 0.2f;
    } background;
    bool resizable = false;
    bool movable = false;
};

class UIManager {
public:
    UIManager(const std::string& ui_file);
    void render();
    void set_state(const std::string& key, int value);
    int get_state(const std::string& key) const;
    const WindowConfig& get_window_config() const { return window_config; }

private:
    std::shared_ptr<UIElement> root;
    std::unordered_map<std::string, int> state;
    WindowConfig window_config;
    
    void render_element(const std::shared_ptr<UIElement>& element);
    void handle_position(const std::string& position);
    void handle_action(const std::string& action);
    std::string format_text(const std::string& text);
    void parse_window_config(const std::shared_ptr<UIElement>& win_element);
}; 