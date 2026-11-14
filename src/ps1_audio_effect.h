#ifndef PS1_AUDIO_EFFECT_H
#define PS1_AUDIO_EFFECT_H

#include <godot_cpp/classes/audio_effect.hpp>
#include <godot_cpp/classes/audio_effect_instance.hpp>
#include <godot_cpp/classes/audio_frame.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <cstdint>

namespace godot {

class PS1AudioEffect;

class PS1AudioEffectInstance : public AudioEffectInstance {
    GDCLASS(PS1AudioEffectInstance, AudioEffectInstance)

    friend class PS1AudioEffect;

    Ref<PS1AudioEffect> base;

protected:
    static void _bind_methods();

public:
    void _process(const void *p_src_buffer, AudioFrame *p_dst_buffer,
            int32_t p_frame_count) override;
};

class PS1AudioEffect : public AudioEffect {
    GDCLASS(PS1AudioEffect, AudioEffect)

    friend class PS1AudioEffectInstance;

    int bit_depth = 10;
    int sample_rate = 22050;
    float low_pass_hz = 10000.0f;
    float drive = 1.15f;
    bool enabled = true;

protected:
    static void _bind_methods();

public:
    void set_bit_depth(int p_value);
    int get_bit_depth() const;

    void set_sample_rate(int p_value);
    int get_sample_rate() const;

    void set_low_pass_hz(float p_value);
    float get_low_pass_hz() const;

    void set_drive(float p_value);
    float get_drive() const;

    void set_enabled(bool p_value);
    bool get_enabled() const;

    Ref<AudioEffectInstance> _instantiate() override;
};

} // namespace godot

#endif // PS1_AUDIO_EFFECT_H
