#include "session_data.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <spdlog/spdlog.h>
#include "../../engine/component/health_component.h"
namespace game::data
{
    void SessionData::addScore(int score)
    {
        current_player_score += score;
        if (current_player_score > max_player_score)
        {
            max_player_score = current_player_score;
        }
        notifyObservers(ScoreChangeEventArgs(current_player_score));
    }

    void SessionData::reset()
    {
        current_player_health = max_player_health;
        current_player_score = 0;
        level_health = max_player_health;
        level_score = 0;
        is_win_ = false;
        current_level_path = "assets/maps/level1.tmj";
    }

    void SessionData::nextLevel(std::string_view level_path)
    {
        current_level_path = level_path;
        level_health = current_player_health;
        level_score = current_player_score;
    }

    bool SessionData::syncMaxScore(std::string_view file_path)
    {
        try
        {
            std::filesystem::path path(file_path);
            std::fstream file(path);
            if (!file.is_open())
            {
                spdlog::warn("Session data file not found: {}", file_path);
                return false;
            }
            nlohmann::json j;
            file >> j;
            auto max_score = j.value("max_player_score", 0);
            if (max_score > max_player_score)
            {
                max_player_score = max_score;
            }
            else
            {
                j["max_player_score"] = max_player_score;
                file.seekp(0);
                file << j.dump(4);
            }
            file.close();
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to load max score from file {}: {}", file_path, e.what());
            return false;
        }
    }

    void SessionData::loadFromFile(std::string_view file_path)
    {
        try
        {
            std::filesystem::path path(file_path);
            std::ifstream file(path);
            if (!file.is_open())
            {
                spdlog::warn("Session data file not found: {}", file_path);
                return;
            }
            nlohmann::json j;
            file >> j;
            max_player_health = j.value("max_player_health", 3);
            max_player_score = std::max(max_player_score, j.value("max_player_score", 0));
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

    void SessionData::saveToFile(std::string_view file_path) const
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
            std::filesystem::path path(file_path);
            std::ofstream file(path);
            file << j.dump(4);
            file.close();
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to save session data to file {}: {}", file_path, e.what());
        }
    }

    void SessionData::onNotify(const engine::interface::EventArgs &event_args)
    {
        if (const auto *health_event = dynamic_cast<const engine::component::HealthChangeEventArgs *>(&event_args))
        {
            current_player_health = health_event->current_health;
            spdlog::info("SessionData: Player health changed to {}", current_player_health);
        }
        else if (const auto *max_health_event = dynamic_cast<const engine::component::MaxHealthChangeEventArgs *>(&event_args))
        {
            max_player_health = max_health_event->max_health;
            spdlog::info("SessionData: Player max health changed to {}", max_player_health);
        }
    }
} // namespace game::data