#pragma once

#include "core/engines/engine.hpp"

#include "core/audio/faust.hpp"
#include "core/audio/voice_manager.hpp"

#include <array>
#include <optional>

namespace otto::engines {

  using namespace core;
  using namespace core::engines;
  using namespace props;

  struct Euclid : SequencerEngine {
    static constexpr int max_length = 16;

    struct Channel : Properties<> {
      Property<int> length = {this, "Length", max_length, has_limits::init(0, max_length),
                                    steppable::init(1)};
      Property<int, wrap> hits = {this, "Hits", 0, has_limits::init(0, max_length), steppable::init(1)};
      Property<int> rotation = {this, "Rotation", 0, has_limits::init(0, max_length),
                                steppable::init(1)};

      std::array<char, 6> notes = {{-1, -1, -1, -1, -1, -1}};

      void update_notes();

      Channel(properties_base* prnt, int n) : Properties(prnt, std::to_string(n)){};

      int _beat_counter = 0;
      std::array<bool, max_length> _hits_enabled;
    };

    struct Props : Properties<> {
      Property<int> channel = {this, "Channel", 0, has_limits::init(0, 3)};
      Properties<> channels_props = {this, "Channels"};
      std::array<Channel, 4> channels = util::generate_array<4>([this](int n) { return Channel(this, n); });

    } props;

    Euclid();

    audio::ProcessData<0> process(audio::ProcessData<0>) override;

    Channel& current_channel()
    {
      return props.channels.at(props.channel);
    }

    std::optional<std::array<char, 6>> recording = std::nullopt;

    bool running = false;

  private:
    friend struct EuclidScreen;

    int _samples_per_beat = 22050;
    int _counter = _samples_per_beat;
    bool _should_run = false;

    // Used in recording to clear the current value when the first keyonevent is sent
    bool _has_pressed_keys = false;
  };
} // namespace otto::engines
