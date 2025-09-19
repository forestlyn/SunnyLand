#pragma once
#include <string>
namespace game::data
{
    class SessionData
    {
    private:
        int max_player_health = 3;
        int max_player_score = 0;
        int current_player_health = 3;
        int current_player_score = 0;

        std::string current_level_path = "assets/maps/level1.tmj";
        int level_health = 3; // 进入关卡时的生命值
        int level_score = 0;  // 进入关卡时的分数

    public:
        SessionData() = default;
        ~SessionData() = default;
        SessionData(const SessionData &) = delete;
        SessionData &operator=(const SessionData &) = delete;
        SessionData(SessionData &&) = delete;
        SessionData &operator=(SessionData &&) = delete;

        // Getter methods
        int getMaxPlayerHealth() const { return max_player_health; }
        int getCurrentPlayerHealth() const { return current_player_health; }
        int getMaxPlayerScore() const { return max_player_score; }
        int getCurrentPlayerScore() const { return current_player_score; }
        std::string getCurrentLevelPath() const { return current_level_path; }
        int getLevelHealth() const { return level_health; }
        int getLevelScore() const { return level_score; }

        // Setter methods
        void setMaxPlayerHealth(int health) { max_player_health = health; }
        void setCurrentPlayerHealth(int health) { current_player_health = health; }
        void setMaxPlayerScore(int score) { max_player_score = score; }
        void setCurrentPlayerScore(int score) { current_player_score = score; }
        void setCurrentLevelPath(const std::string &path) { current_level_path = path; }
        void setLevelHealth(int health) { level_health = health; }
        void setLevelScore(int score) { level_score = score; }

        void addScore(int score);

        void reset();
        void nextLevel(const std::string &level_path);

        void loadFromFile(const std::string &file_path);
        void saveToFile(const std::string &file_path) const;
    };
}