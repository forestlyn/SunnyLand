#include "audio_component.h"
#include "../audio/audio_player.h"
#include "../render/camera.h"
#include "transform_component.h"
#include "../../engine/object/game_object.h"
#include <spdlog/spdlog.h>

namespace engine::component
{
    AudioComponent::AudioComponent(engine::audio::AudioPlayer *audio_player, engine::render::Camera *camera)
        : audio_player_(audio_player), camera_(camera)
    {
        if (!audio_player_ || !camera_)
        {
            spdlog::error("AudioComponent requires a valid AudioPlayer and Camera.");
        }
        spdlog::trace("AudioComponent created!");
    }

    void AudioComponent::init()
    {
        transform_ = m_gameObject->getComponent<engine::component::TransformComponent>();
        if (!transform_)
        {
            spdlog::warn("AudioComponent requires TransformComponent to use spatial audio.");
        }
        spdlog::trace("AudioComponent initialized.");
    }

    void AudioComponent::addSound(const std::string &sound_id, const std::string &file_path)
    {
        sounds_map[sound_id] = file_path;
    }

    void AudioComponent::playSound(const std::string &sound_id, int channels, bool useSpatial)
    {
        auto it = sounds_map.find(sound_id);
        if (it != sounds_map.end())
        {
            if (useSpatial && transform_ && camera_)
            {
                // 计算声音与摄像机的相对位置
                glm::vec2 sound_pos = transform_->getPosition();
                glm::vec2 camera_pos = camera_->getPosition() + camera_->getViewportSize() / 2.0f; // 摄像机中心位置
                glm::vec2 relative_pos = sound_pos - camera_pos;

                // 计算距离和角度
                float distance = glm::length(relative_pos);
                float max_distance = 150.0f; // 最大听觉距离，可以根据需要调整
                if (distance < max_distance)
                {
                    // 在最大距离内，播放声音
                    audio_player_->playSound(it->second, channels);
                }
            }
            else
            {
                // 不使用空间音频，直接播放
                audio_player_->playSound(it->second, channels);
            }
        }
    }

}