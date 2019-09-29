#include "nuclear.hpp"

#include "core/ui/vector_graphics.hpp"

namespace otto::engines {

  using namespace ui;
  using namespace ui::vg;

  /*
   * Declarations
   */
  // Wave parameter logic
  WaveParams::WaveParams(){
    center = {
      {1, 0.01, 0},
      {1, 0.99, 0},
      {1, 0.99, 1},
      {1, 0.01, 1},
      {0.2, 0.01, 1},
      {0.2, 0.99, 1},
      {0.1, 0.99, 0},
      {-0.5, 0.7, 0},
      {-0.5, 0.7, 1},
      {-0.5, 0.01, 0}
    };
    //Max LFO amounts
    deviation = {
      {0.1, 0.1, 0.1},
      {0.1, 0.1, 0.1},
      {0.1, 0.1, 0.1},
      {0.1, 0.1, 0.1},
      {0.1, 0.1, 0.1},
      {0.1, 0.1, 0.1},
      {0.1, 0.1, 0.1},
      {0.1, 0.1, 0.1},
      {0.1, 0.1, 0.1},
      {0.1, 0.1, 0.1}
    };
    
  }
  void WaveParams::set_center(float val){
    //Todo: Find a less hacky way of making val=1 work
    val = std::clamp(val,0.f, 0.9999f) * center.size();

    lower_ = (int)std::floor(val) % center.size();
    upper_ = (lower_ + 1) % center.size();
    frac_ = val - (float)lower_;

    ///Get center value
    //Transformation function
    auto linear_interp = [this](float a, float b){return a * (1 - frac_) + b * frac_; };
    util::transform(center[lower_], center[upper_].begin(), cur_center_.begin(), linear_interp);
  }

  auto WaveParams::get_params(float lfo_value){
     ///Get deviation
     //Transformation function
    auto linear_interp = [this](float a, float b){return a * (1 - frac_) + b * frac_; };
    param_point deviation_results;
    util::transform(deviation[lower_], deviation[upper_].begin(), deviation_results.begin(), linear_interp);

    ///Add contributions in-place
    util::transform(cur_center_, deviation_results.begin(), deviation_results.begin(), 
          [lfo_value](float a, float b){ return a + lfo_value * b;}
    );
    return deviation_results;
  }

  // Screen
  struct NuclearSynthScreen : EngineScreen<NuclearSynth> {
  void draw(Canvas& ctx) override;
  bool keypress(Key key) override;
  void encoder(EncoderEvent e) override;

  using EngineScreen<NuclearSynth>::EngineScreen;
  };

  // NuclearSynth ////////////////////////////////////////////////////////////////

  NuclearSynth::NuclearSynth()
    : SynthEngine<NuclearSynth>(std::make_unique<NuclearSynthScreen>(this)), voice_mgr_(props)
  {}

  float NuclearSynth::Voice::operator()() noexcept
  {
    osc.freq(frequency());
    //Get square/saw sample
    auto pls = osc.pulse();
    //Get tri sample
    auto d = props.morph / M_PI_2;
    auto cycles_per_sample = frequency() / services::AudioManager::current().samplerate();
    auto tri_scale = 2 * cycles_per_sample/ (d * (1 - d));

    auto tri = osc.integrated_quick() * tri_scale;
    //Get amp envelope
    auto env = envelope();
    // Set amp frequency
    filter.set(props.filt_freq + env * props.env_amount);
    // Filter oscillators
    //return filter(pls);
    return tri * props.mix + pls * (1 - props.mix);
    //else return osc.sawtri_quick();
  }

  NuclearSynth::Voice::Voice(Pre& pre) noexcept : VoiceBase(pre) {
    
    props.morph.on_change().connect(
        [this](float m) { osc.morph(m); });
    props.pw.on_change().connect(
        [this](float pw) { osc.pulsewidth(pw); });
    
    
    ///Filter
    //Set change handler
    props.filt_freq.on_change().connect(
      [this](float fr) {
        filter.set(fr);
      }
    ).call_now();
  }

  void NuclearSynth::Voice::on_note_on(float freq_target) noexcept 
  {
    
  }

  NuclearSynth::Pre::Pre(Props& props) noexcept : PreBase(props)
  {
    lfo.freq(1);
    props.wave.on_change().connect(
      [this](float w){
        wave_params.set_center(w);
      }
    ).call_now();
  }

  void NuclearSynth::Pre::operator()() noexcept 
  {
    lfo_value = lfo.tri();
    auto new_params = wave_params.get_params(lfo_value);
    this->props.morph.set(new_params[0]);
    this->props.pw.set(new_params[1]);
    this->props.mix.set(new_params[2]);
  }

  /// Constructor. Takes care of linking appropriate variables to props
  NuclearSynth::Post::Post(Pre& pre) noexcept : PostBase(pre) 
  {

  }

  float NuclearSynth::Post::operator()(float in) noexcept
  {
    return in;
  }

  audio::ProcessData<1> NuclearSynth::process(audio::ProcessData<1> data)
  {
    return voice_mgr_.process(data);
  }

  /*
   * NuclearSynthScreen
   */

  bool NuclearSynthScreen::keypress(Key key)
  {
    switch (key) {
    case ui::Key::blue_click: break;
    default: return false; ;
    }
    return true;
  }

  void NuclearSynthScreen::encoder(EncoderEvent e)
  {
    switch (e.encoder) {
    case Encoder::blue:  engine.props.wave.step(e.steps); break;
    case Encoder::green:  engine.props.pw.step(e.steps); break;
    case Encoder::yellow: engine.props.mix.step(e.steps); break;
    //case Encoder::yellow: engine.props.filt_freq.step(e.steps); break;
    case Encoder::red: engine.props.env_amount.step(e.steps); break;
    }
  }

  void NuclearSynthScreen::draw(ui::vg::Canvas& ctx)
  {
    using namespace ui::vg;

    ctx.font(Fonts::Norm, 35);
    constexpr float x_pad = 30;
    constexpr float y_pad = 50;
    constexpr float space = (height - 2.f * y_pad) / 3.f;

    ctx.beginPath();
    ctx.fillStyle(Colours::Blue);
    ctx.textAlign(HorizontalAlign::Left, VerticalAlign::Middle);
    ctx.fillText("Morph", {x_pad, y_pad});

    ctx.beginPath();
    ctx.fillStyle(Colours::Blue);
    ctx.textAlign(HorizontalAlign::Right, VerticalAlign::Middle);
    ctx.fillText(fmt::format("{:1.2}", engine.props.morph), {width - x_pad, y_pad});

    ctx.beginPath();
    ctx.fillStyle(Colours::Green);
    ctx.textAlign(HorizontalAlign::Left, VerticalAlign::Middle);
    ctx.fillText("PW", {x_pad, y_pad + space});

    ctx.beginPath();
    ctx.fillStyle(Colours::Green);
    ctx.textAlign(HorizontalAlign::Right, VerticalAlign::Middle);
    ctx.fillText(fmt::format("{:1.2}", engine.props.pw), {width - x_pad, y_pad + space});

    ctx.beginPath();
    ctx.fillStyle(Colours::Yellow);
    ctx.textAlign(HorizontalAlign::Left, VerticalAlign::Middle);
    ctx.fillText("Mix", {x_pad, y_pad + 2 * space});

    ctx.beginPath();
    ctx.fillStyle(Colours::Yellow);
    ctx.textAlign(HorizontalAlign::Right, VerticalAlign::Middle);
    ctx.fillText(fmt::format("{:1.3}", engine.props.mix), {width - x_pad, y_pad + 2 * space});

    ctx.beginPath();
    ctx.fillStyle(Colours::Red);
    ctx.textAlign(HorizontalAlign::Left, VerticalAlign::Middle);
    ctx.fillText("Filt. Env.", {x_pad, y_pad + 3 * space});

    ctx.beginPath();
    ctx.fillStyle(Colours::Red);
    ctx.textAlign(HorizontalAlign::Right, VerticalAlign::Middle);
    ctx.fillText(fmt::format("{:1.3}", engine.props.env_amount), {width - x_pad, y_pad + 3 * space});

  }
} // namespace otto::engines
