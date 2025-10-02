#pragma once
#include <string>
#include <string_view>
#include <map>
#include "component.h"
namespace engine::audio
{
    class AudioPlayer;
}

namespace engine::render
{
    class Camera;
}

namespace engine::component
{
    class TransformComponent;
    class AudioComponent : public Component
    {
        friend class engine::object::GameObject;

    private:
        engine::audio::AudioPlayer *audio_player_ = nullptr;
        engine::render::Camera *camera_ = nullptr;
        engine::component::TransformComponent *transform_ = nullptr;

        std::map<std::string, std::string> sounds_map; // sound_id -> sound file path

    public:
        AudioComponent(engine::audio::AudioPlayer *audio_player, engine::render::Camera *camera);
        ~AudioComponent() override = default;
        AudioComponent(const AudioComponent &) = delete;
        AudioComponent &operator=(const AudioComponent &) = delete;
        AudioComponent(AudioComponent &&) = delete;
        AudioComponent &operator=(AudioComponent &&) = delete;

        /// @brief Add a sound to the audio component.
        /// @param sound_id The ID of the sound to add.
        /// @param file_path The file path of the sound to add.
        void addSound(std::string_view sound_id, std::string_view file_path);

        /// @brief Play a sound effect.
        /// @param sound_id The ID of the sound to play.
        /// @param channels The channel to play the sound on (-1 for any available channel).
        /// @param useSpatial Whether to use spatial audio.
        void playSound(std::string_view sound_id, int channels = -1, bool useSpatial = false);

    protected:
        void init() override;
        void update(float, engine::core::Context &) override {}
    };
}