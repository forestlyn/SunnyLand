#pragma once
#include <string>
#include <string_view>
#include "../../engine/interface/subject.h"
#include "../../engine/interface/observer.h"
#include "../../engine/interface/event_args.h"
namespace game::data
{
    class ScoreChangeEventArgs : public engine::interface::EventArgs
    {
    public:
        int current_score;
        ScoreChangeEventArgs(int new_s) : current_score(new_s)
        {
        }
    };
    class SessionData : public engine::interface::Subject, public engine::interface::Observer
    {
    private:
        int max_player_health = 3;
        int max_player_score = 0;
        int current_player_health = 3;
        int current_player_score = 0;
        bool is_win_ = false;

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
        std::string_view getCurrentLevelPath() const { return current_level_path; }
        int getLevelHealth() const { return level_health; }
        int getLevelScore() const { return level_score; }
        bool isWin() const { return is_win_; }

        // Setter methods
        void setMaxPlayerHealth(int health) { max_player_health = health; }
        void setCurrentPlayerHealth(int health) { current_player_health = health; }
        void setMaxPlayerScore(int score) { max_player_score = score; }
        void setCurrentPlayerScore(int score) { current_player_score = score; }
        void setCurrentLevelPath(std::string_view path) { current_level_path = path; }
        void setLevelHealth(int health) { level_health = health; }
        void setLevelScore(int score) { level_score = score; }
        void setIsWin(bool win) { is_win_ = win; }

        void onNotify(const engine::interface::EventArgs &event_args) override;

        void addScore(int score);

        void reset();
        void nextLevel(std::string_view level_path);

        bool syncMaxScore(std::string_view file_path);
        void loadFromFile(std::string_view file_path);
        void saveToFile(std::string_view file_path) const;
    };
}