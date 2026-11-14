#include "ps1_audio_effect.h"

#include <algorithm>

namespace godot {

void PS1AudioEffectInstance::_bind_methods() {
}

void PS1AudioEffectInstance::_process(const void *p_src_buffer,
        AudioFrame *p_dst_buffer, int32_t p_frame_count) {
    if (p_dst_buffer == nullptr || p_frame_count <= 0) {
        return;
    }

    if (p_src_buffer == nullptr) {
        for (int32_t i = 0; i < p_frame_count; ++i) {
            p_dst_buffer[i].left = 0.0f;
            p_dst_buffer[i].right = 0.0f;
        }
        return;
    }

    const AudioFrame *source = static_cast<const AudioFrame *>(p_src_buffer);
    for (int32_t i = 0; i < p_frame_count; ++i) {
        p_dst_buffer[i].left = source[i].left;
        p_dst_buffer[i].right = source[i].right;
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
    ADD_PROPERTY(PropertyInfo(Variant::INT, "sample_rate", PROPERTY_HINT_RANGE, "4000,96000,100"),
            "set_sample_rate", "get_sample_rate");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "low_pass_hz", PROPERTY_HINT_RANGE, "100,20000,100,suffix:Hz"),
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
    if (instance.is_null()) {
        return Ref<AudioEffectInstance>();
    }

    instance->base = Ref<PS1AudioEffect>(this);
    return instance;
}

} // namespace godot
