#include "ps1_audio_effect.h"

#include <algorithm>
#include <cmath>

namespace godot {

static float clamp_sample(float p_value) {
    return std::clamp(p_value, -1.0f, 1.0f);
}

static float apply_drive(float p_sample, float p_drive) {
    if (p_drive <= 1.001f) {
        return p_sample;
    }

    const float normalizer = std::tanh(p_drive);
    if (normalizer <= 0.0f) {
        return p_sample;
    }

    return std::tanh(p_sample * p_drive) / normalizer;
}

void PS1AudioEffectInstance::_bind_methods() {
    // The processing instance is configured by PS1AudioEffect::_instantiate().
}

void PS1AudioEffectInstance::configure(int p_bit_depth, float p_sample_rate,
        float p_low_pass_hz, float p_drive, bool p_enabled, float p_mix_rate) {
    bit_depth = std::clamp(p_bit_depth, 4, 16);
    mix_rate = (p_mix_rate > 1000.0f) ? p_mix_rate : 44100.0f;
    target_sample_rate = std::clamp(p_sample_rate, 1000.0f, mix_rate);
    low_pass_hz = std::clamp(p_low_pass_hz, 100.0f, mix_rate * 0.5f);
    drive = std::clamp(p_drive, 1.0f, 4.0f);
    enabled = p_enabled;

    quantization_levels = std::pow(2.0f, static_cast<float>(bit_depth - 1)) - 1.0f;

    if (low_pass_hz >= mix_rate * 0.499f) {
        low_pass_alpha = 1.0f;
    } else {
        low_pass_alpha = 1.0f - std::exp(-6.28318530718f * low_pass_hz / mix_rate);
    }

    // Capture the first input sample immediately.
    sample_phase = mix_rate;
    filter_left = 0.0f;
    filter_right = 0.0f;
    held_left = 0.0f;
    held_right = 0.0f;
}

bool PS1AudioEffectInstance::_process_silence() const {
    return false;
}

void PS1AudioEffectInstance::_process(const void *p_src_buffer,
        AudioFrame *p_dst_buffer, int32_t p_frame_count) {
    if (p_src_buffer == nullptr || p_dst_buffer == nullptr || p_frame_count <= 0) {
        return;
    }

    const AudioFrame *source = static_cast<const AudioFrame *>(p_src_buffer);

    if (!enabled) {
        for (int32_t i = 0; i < p_frame_count; ++i) {
            p_dst_buffer[i] = source[i];
        }
        return;
    }

    const bool reduce_sample_rate = target_sample_rate < mix_rate - 0.5f;

    for (int32_t i = 0; i < p_frame_count; ++i) {
        const float input_left = source[i].left;
        const float input_right = source[i].right;

        // Smooth before sample holding to tame harsh aliasing.
        filter_left += low_pass_alpha * (input_left - filter_left);
        filter_right += low_pass_alpha * (input_right - filter_right);

        if (!reduce_sample_rate) {
            held_left = filter_left;
            held_right = filter_right;
        } else {
            sample_phase += target_sample_rate;
            if (sample_phase >= mix_rate) {
                sample_phase -= mix_rate;
                held_left = filter_left;
                held_right = filter_right;
            }
        }

        float output_left = apply_drive(held_left, drive);
        float output_right = apply_drive(held_right, drive);

        output_left = std::round(clamp_sample(output_left) * quantization_levels) / quantization_levels;
        output_right = std::round(clamp_sample(output_right) * quantization_levels) / quantization_levels;

        p_dst_buffer[i].left = clamp_sample(output_left);
        p_dst_buffer[i].right = clamp_sample(output_right);
    }
}

void PS1AudioEffect::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_bit_depth", "value"), &PS1AudioEffect::set_bit_depth);
    ClassDB::bind_method(D_METHOD("get_bit_depth"), &PS1AudioEffect::get_bit_depth);
    ClassDB::bind_method(D_METHOD("set_sample_rate", "value"), &PS1AudioEffect::set_sample_rate);
    ClassDB::bind_method(D_METHOD("get_sample_rate"), &PS1AudioEffect::get_sample_rate);
    ClassDB::bind_method(D_METHOD("set_low_pass_hz", "value"), &PS1AudioEffect::set_low_pass_hz);
    ClassDB::bind_method(D_METHOD("get_low_pass_hz"), &PS1AudioEffect::get_low_pass_hz);
    ClassDB::bind_method(D_METHOD("set_drive", "value"), &PS1AudioEffect::set_drive);
    ClassDB::bind_method(D_METHOD("get_drive"), &PS1AudioEffect::get_drive);
    ClassDB::bind_method(D_METHOD("set_enabled", "value"), &PS1AudioEffect::set_enabled);
    ClassDB::bind_method(D_METHOD("get_enabled"), &PS1AudioEffect::get_enabled);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "bit_depth", PROPERTY_HINT_RANGE, "4,16,1"),
            "set_bit_depth", "get_bit_depth");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "sample_rate", PROPERTY_HINT_RANGE, "4000,48000,100"),
            "set_sample_rate", "get_sample_rate");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "low_pass_hz", PROPERTY_HINT_RANGE, "1000,20000,100,suffix:Hz"),
            "set_low_pass_hz", "get_low_pass_hz");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "drive", PROPERTY_HINT_RANGE, "1,4,0.01"),
            "set_drive", "get_drive");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "get_enabled");
}

void PS1AudioEffect::set_bit_depth(int p_value) {
    bit_depth = std::clamp(p_value, 4, 16);
}

int PS1AudioEffect::get_bit_depth() const {
    return bit_depth;
}

void PS1AudioEffect::set_sample_rate(int p_value) {
    sample_rate = std::clamp(p_value, 4000, 96000);
}

int PS1AudioEffect::get_sample_rate() const {
    return sample_rate;
}

void PS1AudioEffect::set_low_pass_hz(float p_value) {
    low_pass_hz = std::clamp(p_value, 100.0f, 48000.0f);
}

float PS1AudioEffect::get_low_pass_hz() const {
    return low_pass_hz;
}

void PS1AudioEffect::set_drive(float p_value) {
    drive = std::clamp(p_value, 1.0f, 4.0f);
}

float PS1AudioEffect::get_drive() const {
    return drive;
}

void PS1AudioEffect::set_enabled(bool p_value) {
    enabled = p_value;
}

bool PS1AudioEffect::get_enabled() const {
    return enabled;
}

Ref<AudioEffectInstance> PS1AudioEffect::_instantiate() {
    Ref<PS1AudioEffectInstance> instance;
    instance.instantiate();
    instance->base = Ref<PS1AudioEffect>(this);

    float current_mix_rate = 44100.0f;
    if (AudioServer::get_singleton() != nullptr) {
        current_mix_rate = static_cast<float>(AudioServer::get_singleton()->get_mix_rate());
    }

    instance->configure(bit_depth, static_cast<float>(sample_rate), low_pass_hz,
            drive, enabled, current_mix_rate);
    return instance;
}

} // namespace godot
