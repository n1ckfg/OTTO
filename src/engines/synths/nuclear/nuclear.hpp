#pragma once

#include "core/engine/engine.hpp"

#include "core/voices/voice_manager.hpp"

#include "util/dsp/DoubleFilter.hpp"
#include <Gamma/Oscillator.h>
#include "util/dsp/oscillators.hpp"

#include "util/reflection.hpp"

namespace otto::engines {

  using namespace core;
  using namespace core::engine;
  using namespace props;

  struct NuclearSynth final : SynthEngine<NuclearSynth> {
    static constexpr util::string_ref name = "Nuclear";

    struct Props {
      Property<float> filter = {1, limits(0, 1), step_size(0.01)};
      Property<bool> osc_type = {false};
      Property<float> morph = {1, limits(-1, 1), step_size(0.01)};
      Property<float> pw = {M_PI_2, limits(0.01, M_PI_2), step_size(0.01)};
      
      Property<float> filt_freq = {1, limits(0, 3.99), step_size(0.01)};
      Property<float> env_amount = {0, limits(-1, 1), step_size(0.01)};

      DECL_REFLECTION(Props, filter);
    } props;

    NuclearSynth();

    audio::ProcessData<1> process(audio::ProcessData<1>) override;

    voices::IVoiceManager& voice_mgr() override
    {
      return voice_mgr_;
    }

    DECL_REFLECTION(NuclearSynth, props, ("voice_manager", &NuclearSynth::voice_mgr_));

  private:
    struct Pre : voices::PreBase<Pre, Props> {
      //gam::AccumPhase<> lfo;

      Pre(Props&) noexcept;

      void operator()() noexcept;
    };

    struct Voice : voices::VoiceBase<Voice, Pre> {
      MultiOsc<> osc;

      DoubleFilter<> filter;

      Voice(Pre&) noexcept;

      float operator()() noexcept;

      void on_note_on(float freq_target) noexcept;
    };

    struct Post : voices::PostBase<Post, Voice> {

      Post(Pre&) noexcept;

      float operator()(float) noexcept;
    };

    voices::VoiceManager<Post, 6> voice_mgr_;
  };

} // namespace otto::engines
