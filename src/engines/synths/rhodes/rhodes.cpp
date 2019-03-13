#include "rhodes.hpp"

#include "core/ui/vector_graphics.hpp"

namespace otto::engines {

  using namespace ui;
  using namespace ui::vg;

  /*
   * Declarations
   */

  struct RhodesSynthScreen : EngineScreen<RhodesSynth> {
    void draw(Canvas& ctx) override;
    bool keypress(Key key) override;
    void rotary(RotaryEvent e) override;

    using EngineScreen<RhodesSynth>::EngineScreen;
  };

  // RhodesSynth ////////////////////////////////////////////////////////////////

  RhodesSynth::RhodesSynth()
    : SynthEngine("Rhodes", props, std::make_unique<RhodesSynthScreen>(this)), voice_mgr_(props)
  {}

  //Voice
  float RhodesSynth::Voice::operator()() noexcept
  {
    reson.freq(frequency());
    float excitation = lpf(exciter() * (1 + noise()));
    float harmonics = env() * overtones();
    float orig_note = reson(excitation*hammer_strength);
    float aux = tanh(0.3*orig_note + props.asymmetry);
    return pickup_hpf(pow(2, 10*aux)) + harmonics;
  }

  RhodesSynth::Voice::Voice(Pre& pre) noexcept : VoiceBase(pre) {
    reson.type(gam::RESONANT);
    reson.res(1500);

    exciter.attack(0.001);
    exciter.curve(0);

    lpf.type(gam::LOW_PASS);
    pickup_lpf.type(gam::LOW_PASS);
    pickup_lpf.freq(1000);
    pickup_hpf.type(gam::HIGH_PASS);

    overtones.resize(1024);
    overtones.addSine(7, 1, 0);
    overtones.addSine(20, 0.5, 0);

    env.decay(3);
  }

  void RhodesSynth::Voice::on_note_on() noexcept {
    reson.zero();
    exciter.decay(1.f/frequency());
    exciter.reset();

    hammer_strength = pow(2, (1 - props.aggro*velocity()*(-3.0)));

    noise.seed(123);

    lpf.freq(pow(velocity()*90*props.aggro + 20,2));
    lpf.zero();

    pickup_hpf.freq(frequency());

    overtones.freq(frequency());
    env.reset(1.7);
  }

  //Preprocessor
  RhodesSynth::Pre::Pre(Props& props) noexcept : PreBase(props)
  {

  }

  void RhodesSynth::Pre::operator()() noexcept {}

  //Postprocessor
  /// Constructor. Takes care of linking appropriate variables to props
  RhodesSynth::Post::Post(Pre& pre) noexcept : PostBase(pre)
  {

    props.lfo_depth.on_change().connect([this](float depth) {
        lfo_amount = depth*0.6;
    });
    props.lfo_speed.on_change().connect([this](float speed) {
        lfo.freq(speed*10);
    });
  }

  float RhodesSynth::Post::operator()(float in) noexcept
  {
    return 0.01*in*(1 + lfo_amount*lfo.tri());
  }

  audio::ProcessData<1> RhodesSynth::process(audio::ProcessData<1> data)
  {
    return voice_mgr_.process(data);
  }

  /*
   * RhodesSynthScreen
   */

  bool RhodesSynthScreen::keypress(Key key)
  {
    return false;
  }

  void RhodesSynthScreen::rotary(RotaryEvent e)
  {

    switch (e.rotary) {
    case Rotary::blue:  engine.props.aggro.step(e.clicks); break;
    case Rotary::green:  engine. props.asymmetry.step(e.clicks); break;
    case Rotary::yellow: engine.props.lfo_speed.step(e.clicks); break;
    case Rotary::red: engine.props.lfo_depth.step(e.clicks); break;
    }

  }

  void RhodesSynthScreen::draw(ui::vg::Canvas& ctx)
  {
    using namespace ui::vg;
    using namespace core::ui::vg;

    //shift = Application::current().ui_manager->is_pressed(ui::Key::shift);

          // Rhodes/Depth/Digits
          ctx.save();
          ctx.font(Fonts::Norm, 40);
          ctx.fillStyle(Colours::Blue);
          ctx.fillText(fmt::format("{:1}", engine.props.lfo_depth), 17.9, 106.9);

          // Rhodes/Depth/Text
          ctx.font(Fonts::Norm, 25);
          ctx.fillStyle(Colours::White);
          ctx.fillText("depth", 17.9, 66.1);

          // Rhodes/Speed
          ctx.restore();

          // Rhodes/Speed/Digits
          ctx.save();
          ctx.font(Fonts::Norm, 40);
          ctx.fillStyle(Colours::Green);
          ctx.fillText(fmt::format("{:1}", engine.props.lfo_speed), 17.9, 205.7);

          // Rhodes/Speed/Text
          ctx.font(Fonts::Norm, 25);
          ctx.fillStyle(Colours::White);
          ctx.fillText("speed", 17.9, 164.9);

          // (temp unused) movement values
          float aggroMultiplier = engine.props.aggro * engine.props.aggro;
          float valW1 = 1- ((engine.props.aggro * aggroMultiplier)*30);
          float valR1 = 1- ((engine.props.aggro * aggroMultiplier)*20);
          float valW2 = 1- ((engine.props.aggro * aggroMultiplier)*10);
          // R2 does not move
          float valW3 = 1+ ((engine.props.aggro * aggroMultiplier)*10);
          float valR3 = 1+ ((engine.props.aggro * aggroMultiplier)*20);
          float valW4 = 1+ ((engine.props.aggro * aggroMultiplier)*30);
          // value for altering arc
          float arcVal1 = 0.2 + (engine.props.aggro / 6); //red bot arc       | right
          float arcVal2 =       (engine.props.aggro / 2); //white bot arc     | about right, too much at (6) better at (4)
          float arcVal3 =   1 + (engine.props.aggro * 3); //lineto            | needs to be a bit less + 123 grad
          float arcVal4 =   1 + (engine.props.aggro * 3); //red bot lineto    | needs to be gradual
          float arcVal5 =   1 + (engine.props.aggro / 5); //red top arc       | needs to be a bit tighter(3)
          float arcVal6 =   1 + (engine.props.aggro / 6); //white top arc     | needs to be less (3) better at (6)
          float arcVal7 =   1 + (engine.props.aggro * 2); //white lineto      | about right, tiny bit less

          //
          //
          // !!!
          // anchor + chain supergroup
          ctx.group([&] {
            ctx.translate(0, (1.f + engine.props.aggro * 10));
            // Rhodes/AnchorHack
            ctx.group([&] {
              ctx.translate(0,valW4);
              ctx.beginPath();
              ctx.moveTo(271.0, 123.3);
              ctx.bezierCurveTo(273.2, 120.6, 277.2, 120.2, 279.9, 122.3);
              ctx.bezierCurveTo(282.6, 124.5, 283.0, 128.4, 280.9, 131.1);
              ctx.lineWidth(6.0);
              ctx.strokeStyle(Colours::Yellow);
              ctx.lineCap(Canvas::LineCap::ROUND);
              ctx.lineJoin(Canvas::LineJoin::ROUND);
              ctx.stroke();
            });

            ctx.group([&] {
              // move entire group sideways.
              ctx.translate(20,0);
              //////////////////////////
              // RED CHAIN 1 /////////
              //////////////////////////

              ctx.group([&] {
                ctx.translate(0,valR1);
                ctx.beginPath();
                ctx.arc(130, 86, 15, arcVal1, 1 * M_PI);
                ctx.lineTo(115,(85 - arcVal4));
                ctx.lineWidth(6.0);
                ctx.strokeStyle(Colours::Red);
                ctx.lineCap(Canvas::LineCap::ROUND);
                ctx.lineJoin(Canvas::LineJoin::ROUND);
                ctx.stroke();

                ctx.beginPath();
                ctx.arc(130, 61, 15, arcVal5 * M_PI, 0);
                ctx.lineTo(145,(62 - arcVal4));
                // x,y,radius,start(rad),end(rad),counterclock
                ctx.lineWidth(6.0);
                ctx.strokeStyle(Colours::Red);
                ctx.stroke();
              });
              //////////////////////////
              // RED CHAIN 2 /////////
              //////////////////////////

              ctx.group([&] {
                ctx.translate(0,0);
                ctx.beginPath();
                ctx.arc(178, 86, 15, arcVal1, 1 * M_PI);
                ctx.lineTo(163,(85 - arcVal4));
                ctx.lineWidth(6.0);
                ctx.strokeStyle(Colours::Red);
                ctx.stroke();

                ctx.beginPath();
                ctx.arc(178, 61, 15, arcVal5 * M_PI, 0);
                ctx.lineTo(193,(62 + arcVal4));
                // x,y,radius,start(rad),end(rad),counterclock
                ctx.lineWidth(6.0);
                ctx.strokeStyle(Colours::Red);
                ctx.stroke();
              });
              //////////////////////////
              // RED CHAIN 3 /////////
              //////////////////////////

              ctx.group([&] {
                ctx.translate(0,valR3);
                ctx.beginPath();
                ctx.arc(225, 86, 15, arcVal1, 1 * M_PI);
                ctx.lineTo(210,(85 - arcVal4));
                ctx.lineWidth(6.0);
                ctx.strokeStyle(Colours::Red);
                ctx.stroke();

                //top
                ctx.beginPath();
                ctx.arc(225, 61, 15, arcVal5 * M_PI, 0);
                ctx.lineTo(240,(62 + arcVal4));
                // x,y,radius,start(rad),end(rad),counterclock
                ctx.lineWidth(6.0);
                ctx.strokeStyle(Colours::Red);
                ctx.stroke();
              });
              //////////////////////////
              // WHITE CHAIN 1 /////////
              //////////////////////////

              ctx.group([&] {
                ctx.translate(0,valW1);
                ctx.beginPath();
                ctx.arc(106, 110, 15, arcVal2, 1 * M_PI);
                ctx.lineTo(91,82);
                ctx.strokeStyle(Colours::White);
                ctx.stroke();

                ctx.beginPath();
                ctx.arc(106, 82, 15, 1 * M_PI, 0);
                ctx.lineTo(121,(83 + arcVal7));
                // x,y,radius,start(rad),end(rad),counterclock
                ctx.strokeStyle(Colours::White);
                ctx.stroke();
              });
              //////////////////////////
              // WHITE CHAIN 2 /////////
              //////////////////////////

              ctx.group([&] {
                ctx.translate(0,valW2);
                ctx.beginPath();
                ctx.arc(154, 110, 15, arcVal2, 1 * M_PI);
                // lT moves when altered
                ctx.lineTo(139,(109 - arcVal3));
                ctx.strokeStyle(Colours::White);
                ctx.stroke();

                ctx.beginPath();
                ctx.arc(154, 82, 15, arcVal6 * M_PI, 0);
                ctx.lineTo(169,(83 + arcVal7));
                // x,y,radius,start(rad),end(rad),counterclock
                ctx.strokeStyle(Colours::White);
                ctx.stroke();
              });
              //////////////////////////
              // WHITE CHAIN 3 /////////
              //////////////////////////

              ctx.group([&] {
                ctx.translate(0,valW3);
                ctx.beginPath();
                ctx.arc(201, 110, 15, arcVal2, 1 * M_PI);
                // lT moves when altered
                ctx.lineTo(186,(109 - arcVal3));
                ctx.strokeStyle(Colours::White);
                ctx.stroke();

                ctx.beginPath();
                ctx.arc(201, 82, 15, arcVal6 * M_PI, 0);
                ctx.lineTo(216,(83 + arcVal7));
                // x,y,radius,start(rad),end(rad),counterclock
                ctx.strokeStyle(Colours::White);
                ctx.stroke();
              });
              //////////////////////////
              // WHITE CHAIN 4 /////////
              //////////////////////////

              ctx.group([&] {
                ctx.translate(0,valW4);
                ctx.beginPath();
                ctx.arc(249, 110, 15, 0, 1 * M_PI);
                // lT moves when altered
                ctx.lineTo(234,(109 - arcVal3));
                ctx.strokeStyle(Colours::White);
                ctx.stroke();

                ctx.beginPath();
                ctx.arc(249, 82, 15, arcVal6 * M_PI, 0);
                ctx.lineTo(264,110);
                // x,y,radius,start(rad),end(rad),counterclock
                ctx.strokeStyle(Colours::White);
                ctx.stroke();
              });
            });

            // anchor group
            ctx.group([&] {
              ctx.translate(0,valW4);
              ctx.beginPath();
              ctx.moveTo(280.9, 131.1);
              ctx.bezierCurveTo(278.7, 133.8, 274.7, 134.2, 272.0, 132.1);
              ctx.bezierCurveTo(269.3, 130.0, 268.9, 126.0, 271.0, 123.3);
              ctx.strokeStyle(Colours::Yellow);
              ctx.stroke();

              // Rhodes/Anchor/R
              ctx.beginPath();
              ctx.moveTo(288.5, 153.0);
              ctx.bezierCurveTo(288.5, 156.5, 285.7, 159.3, 282.2, 159.3);
              ctx.bezierCurveTo(278.8, 159.3, 276.0, 156.5, 276.0, 153.0);
              ctx.stroke();

              // Rhodes/Anchor/Stem
              ctx.beginPath();
              ctx.moveTo(276.0, 133.4);
              ctx.lineTo(276.0, 153.0);
              ctx.stroke();

              // Rhodes/Anchor/L
              ctx.beginPath();
              ctx.moveTo(276.0, 153.0);
              ctx.bezierCurveTo(276.0, 156.5, 273.2, 159.3, 269.7, 159.3);
              ctx.bezierCurveTo(266.3, 159.3, 263.5, 156.5, 263.5, 153.0);
              ctx.stroke();
            });

            // Rhodes/Waves
            ctx.group([&] {
              ctx.beginPath();
              ctx.moveTo(106, 188);
              // x1, y1, x2, y2, x, y
              ctx.bezierCurveTo(188, 200, 200, 200, 200, 188);
              ctx.strokeStyle(Colours::Blue);
              ctx.stroke();
            });
          });
    ///
  }
} // namespace otto::engines
