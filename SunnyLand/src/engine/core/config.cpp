#include <nlohmann/json.hpp>
#include "config.h"
#include <spdlog/spdlog.h>
#include <fstream>

namespace engine::core
{
    Config::Config(const std::string &config_file_path)
    {
        if (!loadFromFile(config_file_path))
        {
            saveToFile(config_file_path); // Save default config if loading fails
            spdlog::warn("Failed to load config from file: {}, using default values", config_file_path);
        }
    }

    bool Config::loadFromFile(const std::string &file_path)
    {
        try
        {
            std::ifstream file(file_path);
            if (!file.is_open())
            {
                spdlog::error("Could not open config file: {}", file_path);
                return false;
            }

            nlohmann::json j;
            file >> j;
            fromJson(j);
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Error loading config file: {}, error: {}", file_path, e.what());
            return false;
        }
    }

    void Config::saveToFile(const std::string &file_path) const
    {
        try
        {
            nlohmann::ordered_json j = toJson();
            std::ofstream file(file_path);
            if (!file.is_open())
            {
                spdlog::error("Could not open config file for writing: {}", file_path);
                return;
            }
            file << j.dump(4); // Pretty print with 4 spaces
        }
        catch (const std::exception &e)
        {
            spdlog::error("Error saving config to file: {}, error: {}", file_path, e.what());
        }
    }

    void Config::fromJson(const nlohmann::json &j)
    {
        if (j.contains("window_title"))
            window_title_ = j["window_title"].get<std::string>();
        if (j.contains("window_size_w"))
            window_size_w_ = j["window_size_w"].get<int>();
        if (j.contains("window_size_h"))
            window_size_h_ = j["window_size_h"].get<int>();
        if (j.contains("window_resizable"))
            window_resizable_ = j["window_resizable"].get<bool>();
        if (j.contains("target_fps"))
            target_fps_ = j["target_fps"].get<int>();
        if (j.contains("vsync_enable"))
            vsync_enable_ = j["vsync_enable"].get<bool>();
        if (j.contains("music_volume"))
            music_volume_ = j["music_volume"].get<float>();
        if (j.contains("sound_volume"))
            sound_volume_ = j["sound_volume"].get<float>();
        if (j.contains("input_mappings"))
            input_mappings_ = j["input_mappings"].get<std::unordered_map<std::string, std::vector<std::string>>>();
    }

    nlohmann::ordered_json Config::toJson() const
    {
        nlohmann::ordered_json j;
        j["window_title"] = window_title_;
        j["window_size_w"] = window_size_w_;
        j["window_size_h"] = window_size_h_;
        j["window_resizable"] = window_resizable_;
        j["target_fps"] = target_fps_;
        j["vsync_enable"] = vsync_enable_;
        j["music_volume"] = music_volume_;
        j["sound_volume"] = sound_volume_;
        j["input_mappings"] = input_mappings_;
        return j;
    }

}