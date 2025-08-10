#pragma once
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include <vector>
#include <string>

namespace engine::core
{
    class Config final
    {
    public:
        std::string window_title_ = "SunnyLand";
        int window_size_w_ = 1280;
        int window_size_h_ = 720;
        bool window_resizable_ = true;

        int target_fps_ = 60;

        bool vsync_enable_ = true;

        float music_volume_ = 0.5f;
        float sound_volume_ = 0.5f;

        std::unordered_map<std::string, std::vector<std::string>> input_mappings_ = {
            // 提供一些合理的默认值，以防配置文件加载失败或缺少此部分
            {"move_left", {"A", "Left"}},
            {"move_right", {"D", "Right"}},
            {"move_up", {"W", "Up"}},
            {"move_down", {"S", "Down"}},
            {"jump", {"J", "Space"}},
            {"attack", {"K", "MouseLeft"}},
            {"pause", {"P", "Escape"}},
            // 可以继续添加更多默认动作
        };

        explicit Config(const std::string &config_file_path);
        Config(const Config &) = delete;
        Config &operator=(const Config &) = delete;
        Config(Config &&) = delete;
        Config &operator=(Config &&) = delete;

        bool loadFromFile(const std::string &file_path);
        void saveToFile(const std::string &file_path) const;

    private:
        void fromJson(const nlohmann::json &j);
        nlohmann::ordered_json toJson() const;
    };
}