#include "nuclear.hpp"

#include "core/ui/vector_graphics.hpp"

namespace otto::engines {

  using namespace ui;
  using namespace ui::vg;

  /*
   * Declarations
   */

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
    //Get oscillator sample
    auto pls = osc.pulse();
    //Get amp envelope
    auto env = envelope();
    // Set amp frequency
    filter.set(props.filt_freq + env * props.env_amount);
    // Filter oscillators
    return filter(pls);
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
    
  }

  void NuclearSynth::Pre::operator()() noexcept {}

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
    case ui::Key::blue_click: engine.props.osc_type = !engine.props.osc_type; break;
    default: return false; ;
    }
    return true;
  }

  void NuclearSynthScreen::encoder(EncoderEvent e)
  {
    switch (e.encoder) {
    case Encoder::blue:  engine.props.morph.step(e.steps); break;
    case Encoder::green:  engine.props.pw.step(e.steps); break;
    case Encoder::yellow: engine.props.filt_freq.step(e.steps); break;
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
    ctx.fillText("Freq", {x_pad, y_pad + 2 * space});

    ctx.beginPath();
    ctx.fillStyle(Colours::Yellow);
    ctx.textAlign(HorizontalAlign::Right, VerticalAlign::Middle);
    ctx.fillText(fmt::format("{:1.3}", engine.props.filt_freq), {width - x_pad, y_pad + 2 * space});

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
