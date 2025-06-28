/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <hal/hal.h>

static constexpr int SAMPLE_RATE = 48000;
static constexpr double PI = 3.14159265358979323846;

// Simple linear envelope: attack + decay
void generate_tone_with_linear_envelope(std::vector<int16_t> &buffer, double freq, double duration, double volume)
{
    int samples = static_cast<int>(SAMPLE_RATE * duration);
    int attack_samples = SAMPLE_RATE * 0.005; // 5ms attack
    for (int i = 0; i < samples; ++i)
    {
        double t = static_cast<double>(i) / SAMPLE_RATE;
        double amplitude = 1.0;

        if (i < attack_samples)
        {
            amplitude = static_cast<double>(i) / attack_samples; // Linear attack
        }
        else
        {
            amplitude = 1.0 - static_cast<double>(i - attack_samples) / (samples - attack_samples); // Linear decay
        }

        double sample = std::sin(2.0 * PI * freq * t) * amplitude;
        int16_t s = static_cast<int16_t>(sample * volume * 32767);
        buffer.push_back(s); // Left
        buffer.push_back(s); // Right
    }
}

namespace audio
{
    void play_tone(int frequency, double durationSec)
    {
        if (GetHAL()->getSpeakerVolume() <= 0)
        {
            return;
        }

        const int sample_rate = 48000;
        const int samples = static_cast<int>(sample_rate * durationSec);
        std::vector<int16_t> buffer(samples * 2); // Stereo

        const int fade_len = 200;                 // Fade-out length (samples)
        const float amplitude = 32767.0f / 5;

        for (int i = 0; i < samples; ++i)
        {
            float amp = amplitude;

            // Apply ending fade-out
            if (i >= samples - fade_len)
            {
                float fade_factor = static_cast<float>(samples - i) / fade_len;
                amp *= fade_factor;
            }

            int16_t value = static_cast<int16_t>(amp * sin(2.0 * M_PI * frequency * i / sample_rate));
            buffer[i * 2] = value;     // Left channel
            buffer[i * 2 + 1] = value; // Right channel
        }

        GetHAL()->audioPlay(buffer);
    }

    void play_melody(const std::vector<int> &midiList, double durationSec = 0.1)
    {
        if (GetHAL()->getSpeakerVolume() <= 0)
        {
            return;
        }

        const int sample_rate = 48000;
        const int samples_per_note = static_cast<int>(sample_rate * durationSec);
        const int fade_len = 200;                               // Fade-out length at the end of each note
        const float amplitude = 32767.0f / 5;

        std::vector<int16_t> buffer;                            // Large buffer to store the entire melody
        buffer.reserve(midiList.size() * samples_per_note * 2); // Reserve space for stereo

        for (int midiNote: midiList)
        {
            for (int i = 0; i < samples_per_note; ++i)
            {
                float amp = amplitude;

                // Apply fade-out (only at the end of each note)
                if (i >= samples_per_note - fade_len)
                {
                    float fade_factor = static_cast<float>(samples_per_note - i) / fade_len;
                    amp *= fade_factor;
                }

                int16_t sample = 0;
                if (midiNote >= 0)
                {
                    double freq = 440.0 * pow(2.0, (midiNote - 69) / 12.0);
                    sample = static_cast<int16_t>(amp * sin(2.0 * M_PI * freq * i / sample_rate));
                }

                buffer.push_back(sample); // Left channel
                buffer.push_back(sample); // Right channel
            }
        }

        GetHAL()->audioPlay(buffer);
    }

    void play_tone_from_midi(int midi, double durationSec)
    {
        if (GetHAL()->getSpeakerVolume() <= 0)
        {
            return;
        }

        double freq = 440.0 * std::pow(2.0, (midi - 69) / 12.0);
        play_tone(static_cast<int>(freq), durationSec);
    }

    void play_random_tone(int semitoneShift = 0, double durationSec = 0.15)
    {
        if (GetHAL()->getSpeakerVolume() <= 0)
        {
            return;
        }

        std::vector<int> scale = {60, 62, 64, 65, 67, 69, 71}; // C major scale (C D E F G A B)

        int index = rand() % scale.size();
        int midi = scale[index] + semitoneShift;

        play_tone_from_midi(midi, durationSec);
    }

    void play_next_tone_progression(double durationSec)
    {
        if (GetHAL()->getSpeakerVolume() <= 0)
        {
            return;
        }

        constexpr int REPEAT_EACH_CHORD = 1;
        constexpr int SEMITONE_SHIFT = 24;

        static std::vector<int> melody = {60, 67, 69, 64, 65, 60, 65, 67}; // 15634145

        static size_t index = 0;
        static int repeat_counter = -1;

        if (++repeat_counter >= REPEAT_EACH_CHORD)
        {
            repeat_counter = 0;
            index++;
        }

        int midi = melody[index % melody.size()] + SEMITONE_SHIFT;

        play_tone_from_midi(midi, durationSec);
    }

    void play_chord(const std::vector<int> &midiNotes, double durationSec)
    {
        if (GetHAL()->getSpeakerVolume() <= 0)
        {
            return;
        }

        std::vector<double> freqs;
        for (int midi: midiNotes)
        {
            double freq = 440.0 * std::pow(2.0, (midi - 69) / 12.0);
            freqs.push_back(freq);
        }

        std::vector<int16_t> buffer;
        std::vector<std::vector<int16_t>> tones;
        for (double freq: freqs)
        {
            std::vector<int16_t> tone;
            generate_tone_with_linear_envelope(tone, freq, durationSec, 0.35);
            tones.push_back(tone);
        }

        size_t num_samples = tones[0].size();
        buffer.resize(num_samples, 0);
        for (size_t i = 0; i < num_samples; ++i)
        {
            int32_t mixed = 0;
            for (auto &tone: tones)
            {
                mixed += tone[i];
            }
            buffer[i] = std::clamp((int) mixed, -32768, 32767);
        }

        GetHAL()->audioPlay(buffer);
    }

    void play_random_chord(int semitoneShift, double durationSec)
    {
        if (GetHAL()->getSpeakerVolume() <= 0)
        {
            return;
        }

        // C major scale
        std::vector<int> scale = {60, 62, 64, 65, 67, 69, 71}; // C D E F G A B

        // Random root and chord structure
        int root_index = rand() % 4; // Leave space for third and fifth
        int root = scale[root_index] + semitoneShift;
        int third = scale[root_index + 2] + semitoneShift;
        int fifth = scale[root_index + 4] + semitoneShift;
        std::vector<int> chord_midi = {root, third, fifth};

        play_chord(chord_midi, durationSec);
    }

    void play_next_chord_progression(double durationSec)
    {
        if (GetHAL()->getSpeakerVolume() <= 0)
        {
            return;
        }

        constexpr int REPEAT_EACH_CHORD = 2;
        constexpr int SEMITONE_SHIFT = 0;

        static std::vector<int> chord_roots = {60, 67, 69, 64, 65, 60, 65, 67}; // 15634145
        // static std::vector<int> chord_roots = { 60, 71, 69, 67, 65, 64, 62, 60 }; // 17654321
        // static std::vector<int> chord_roots = { 65, 67, 64, 69, 62, 67, 60 }; // 4536251
        // static std::vector<int> chord_roots = {65, 65, 64, 69, 62, 67, 60}; // 4436251

        static size_t current_chord_index = 0;
        static int repeat_counter = -1;

        if (++repeat_counter >= REPEAT_EACH_CHORD)
        {
            repeat_counter = 0;
            current_chord_index++;
        }

        int root = chord_roots[current_chord_index % chord_roots.size()] + SEMITONE_SHIFT;

        // Determine if it's a minor chord (only handle Am)
        bool is_minor = (root % 12 == 9); // MIDI 69, 81, etc. are all A

        std::vector<int> chord_midi = {root, root + (is_minor ? 3 : 4), root + 7};

        play_chord(chord_midi, durationSec);
    }

} // namespace audio
