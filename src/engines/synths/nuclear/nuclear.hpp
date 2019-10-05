#pragma once

#include "core/engine/engine.hpp"

#include "core/voices/voice_manager.hpp"
#include "util/algorithm.hpp"
#include "util/dsp/DoubleFilter.hpp"
#include <Gamma/Oscillator.h>
#include "util/dsp/oscillators.hpp"

#include "util/reflection.hpp"

namespace otto::engines {

  using namespace core;
  using namespace core::engine;
  using namespace props;

  class WaveParams{
  using param_point = std::array<float, 3>;
  public:
    WaveParams();
    void set_center(float val);
    auto get_params(float lfo_value, float morph_scale, float pw_scale, float mix_scale);

  private:
    int lower_ = 0;
    int upper_ = 0;
    float frac_ = 0;
    param_point cur_center_;
    std::vector<param_point> center;
    std::vector<param_point> deviation;
  };


  struct NuclearSynth final : SynthEngine<NuclearSynth> {
    static constexpr util::string_ref name = "Nuclear";

    struct Props {
      Property<float> filter = {1, limits(0, 1), step_size(0.01)};

      Property<float, wrap> wave = {0, limits(0, 1), step_size(0.002)};

      Property<float> morph = {1, limits(-1, 1), step_size(0.01)};
      Property<float> pw = {0.99, limits(0.01, 0.99), step_size(0.01)};
      Property<float> mix = {0, limits(0, 1), step_size(0.01)};

      Property<float> morph_scale = {0.1, limits(0,1), step_size(0.01)};
      Property<float> pw_scale = {0.1, limits(0,1), step_size(0.01)};
      Property<float> mix_scale = {0.1, limits(0,1), step_size(0.01)};
      
      Property<float> modulation = {0, limits(0, 1), step_size(0.01)};

      Property<float> filt_freq = {1, limits(0, 3.99), step_size(0.01)};
      Property<float> env_amount = {0, limits(-1, 1), step_size(0.01)};

      DECL_REFLECTION(Props, wave, modulation, filt_freq, env_amount);
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
      gam::LFO<> lfo;
      float lfo_value = 0;
      float mod_amp = 0;

      WaveParams wave_params;

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
