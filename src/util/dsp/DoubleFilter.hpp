#include <Gamma/Filter.h>

///Double filter
template <class Tv=gam::real, class Tp=gam::real, class Td=gam::DomainObserver>
class DoubleFilter {
public:
  void set(float fr) {
    auto filters = get_filter_params(fr);
    //Low-pass
    lpf_.freq(filters.first.first);
    lpf_.res(filters.first.second);
    //High-pass
    hpf_.freq(filters.second.first);
    hpf_.res(filters.second.second);
    //Mixing variables
    get_mixing_params(fr);
  }

  Tv operator()(Tv in) {
    return mL_ * lpf_(in) + mH_ * hpf_(in);
  }

private:
  float mL_ = 1;
  float mH_ = 1;
  auto get_filter_params(float fr);
  auto get_mixing_params(float fr);
  
  gam::Biquad<> lpf_{20.f, 1.f, gam::LOW_PASS};
  gam::Biquad<> hpf_{20.f, 1.f, gam::HIGH_PASS};
};

//Takes float from 0 to 1. Gives frequency from 20 to 20000 
inline float value_to_freq(float val) {
  return 20.f + val * val * val * (20000.f - 20.f);
}

template <class Tv, class Tp, class Td>
auto DoubleFilter<Tv,Tp,Td>::get_filter_params(float fr) {
  //First is frequency, second is resonance
  using pair = std::pair<float,float>;
  pair lpf = {20000.f, 1.f};
  pair hpf = {20.f, 1.f};

  //What is HIGH resonance really??
  constexpr float hi_res = 2.5f;

  float val = fr - std::floor(fr);

  if (fr < 1){
    lpf = {value_to_freq(val), 1.f};
  } else if (fr < 2) {
    hpf = {value_to_freq(val), 1.f};
    if (fr > 1.5f) lpf = {20.f, hi_res};
  } else if (fr < 3) {
    lpf = {value_to_freq(val), hi_res};
    if (fr < 2.5f) hpf = {20000.f, hi_res};
    else hpf = {20.f, hi_res};
  } else {
    lpf = {20000.f, hi_res};
    hpf = {value_to_freq(val), hi_res};
  }
  //DLOGI("lpf: ({},{})", lpf.first, lpf.second);
  //DLOGI("hpf: ({},{})", hpf.first, hpf.second);
  return std::make_pair(lpf, hpf);
}

template <class Tv, class Tp, class Td>
auto DoubleFilter<Tv,Tp,Td>::get_mixing_params(float fr) {
  constexpr float interp_length = 0.2;
  if (fr <= 1 - interp_length){
    mL_ = 1;
  } 
  else if (fr <= 1 + interp_length) {
    float interp = 0.5 * (fr - 1 + interp_length) / interp_length;
    mL_ = 1 - interp;
  } 
  else if (fr <= 2 - interp_length) {
    mL_ = 0;
  } 
  else if (fr <= 2 + interp_length) {
    float interp = 0.5 * (fr - 2 + interp_length)  / interp_length;
    mL_ = interp;
  } 
  else if (fr <= 3 - interp_length) {
    mL_ = 1;
  } 
  else if (fr <= 3 + interp_length) {
    float interp = 0.5 * (fr - 3 + interp_length)  / interp_length;
    mL_ = 1 - interp;
  } else {
    mL_ = 0;
  }
  mH_ = 1 - mL_;
  //DLOGI("mL: {}", mL_);
}
