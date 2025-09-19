#include "session_data.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <spdlog/spdlog.h>
namespace game::data
{
    void SessionData::addScore(int score)
    {
        current_player_score += score;
        if (current_player_score > max_player_score)
        {
            max_player_score = current_player_score;
        }
    }

    void SessionData::reset()
    {
        current_player_health = max_player_health;
        current_player_score = 0;
        level_health = max_player_health;
        level_score = 0;
        current_level_path = "assets/maps/level1.tmj";
    }

    void SessionData::nextLevel(const std::string &level_path)
    {
        current_level_path = level_path;
        current_player_health = level_health;
        current_player_score = level_score;
    }

    void SessionData::loadFromFile(const std::string &file_path)
    {
        try
        {
            std::ifstream file(file_path);
            if (!file.is_open())
            {
                spdlog::warn("Session data file not found: {}", file_path);
                return;
            }
            nlohmann::json j;
            file >> j;
            max_player_health = j.value("max_player_health", 3);
            max_player_score = j.value("max_player_score", 0);
            current_player_health = j.value("current_player_health", 3);
            current_player_score = j.value("current_player_score", 0);
            current_level_path = j.value("current_level_path", "assets/maps/level1.tmj");
            level_health = j.value("level_health", 3);
            level_score = j.value("level_score", 0);
            file.close();
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to load session data from file {}: {}", file_path, e.what());
        }
    }

    void SessionData::saveToFile(const std::string &file_path) const
    {
        try
        {
            nlohmann::json j;
            j["max_player_health"] = max_player_health;
            j["max_player_score"] = max_player_score;
            j["current_player_health"] = current_player_health;
            j["current_player_score"] = current_player_score;
            j["current_level_path"] = current_level_path;
            j["level_health"] = level_health;
            j["level_score"] = level_score;
            std::ofstream file(file_path);
            file << j.dump(4);
            file.close();
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to save session data to file {}: {}", file_path, e.what());
        }
    }
} // namespace game::data