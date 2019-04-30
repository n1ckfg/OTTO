#pragma once
#include <functional>

#include "util/algorithm.hpp"

namespace otto::core::audio {

  template<int N>
  ProcessData<N>::ProcessData(std::array<AudioBufferHandle, channels> audio,
                              midi::shared_vector<midi::AnyMidiEvent> midi,
                              long nframes) noexcept
    : audio(audio), midi(midi), nframes(nframes)
  {}

  template<int N>
  ProcessData<N>::ProcessData(std::array<AudioBufferHandle, channels> audio,
                              midi::shared_vector<midi::AnyMidiEvent> midi) noexcept
    : audio(audio), midi(midi), nframes(audio[0].size())
  {}

  template<int N>
  ProcessData<N>::ProcessData(std::array<AudioBufferHandle, channels> audio) noexcept
    : audio(audio), midi(), nframes(audio[0].size())
  {}

  template<int N>
  ProcessData<0> ProcessData<N>::midi_only()
  {
    return {midi, nframes};
  }

  template<int N>
  ProcessData<N> ProcessData<N>::audio_only()
  {
    return {audio, {}, nframes};
  }

  template<int N>
  template<std::size_t NN>
  ProcessData<NN> ProcessData<N>::redirect(const std::array<AudioBufferHandle, NN>& buf)
  {
    return ProcessData<NN>{buf, midi, nframes};
  }

  template<int N>
  ProcessData<1> ProcessData<N>::redirect(const AudioBufferHandle& buf)
  {
    return ProcessData<1>{buf, midi, nframes};
  }

  /// Get only a slice of the audio.
  ///
  /// \param idx The index to start from
  /// \param length The number of frames to keep in the slice
  ///   If `length` is negative, `nframes - idx` will be used
  /// \requires parameter `idx` shall be in the range `[0, nframes)`, and
  /// `length` shall be in range `[0, nframes - idx]`
  template<int N>
  ProcessData<N> ProcessData<N>::slice(int idx, int length)
  {
    auto res = *this;
    length = length < 0 ? nframes - idx : length;
    res.nframes = length;
    res.audio = audio.slice(idx, length);
    return res;
  }

  template<int N>
  auto ProcessData<N>::raw_audio_buffers() -> std::array<float*, channels>
  {
    return {util::generate_array<channels>([&](int n) { return audio[n].data(); })};
  }

  // ProcessData<0> //

  inline ProcessData<0>::ProcessData(midi::shared_vector<midi::AnyMidiEvent> midi,
                                     long nframes) noexcept
    : midi(midi), nframes(nframes)
  {}

  template<std::size_t NN>
  ProcessData<NN> ProcessData<0>::redirect(const std::array<AudioBufferHandle, NN>& buf)
  {
    return ProcessData<NN>{buf, midi, nframes};
  }

  inline ProcessData<1> ProcessData<0>::redirect(const AudioBufferHandle& buf)
  {
    return ProcessData<1>{buf, midi, nframes};
  }

  inline std::array<float*, 0> ProcessData<0>::raw_audio_buffers()
  {
    return {};
  }

  // ProcessDaata<1> //

  inline ProcessData<1>::ProcessData(AudioBufferHandle audio,
                                     midi::shared_vector<midi::AnyMidiEvent> midi,
                                     long nframes) noexcept
    : audio(audio), midi(midi), nframes(nframes)
  {}

  inline ProcessData<1>::ProcessData(AudioBufferHandle audio,
                                     midi::shared_vector<midi::AnyMidiEvent> midi) noexcept
    : audio(audio), midi(midi), nframes(audio.size())
  {}

  inline ProcessData<1>::ProcessData(AudioBufferHandle audio) noexcept
    : audio(audio), midi(), nframes(audio.size())
  {}

  inline ProcessData<1>::ProcessData(std::array<AudioBufferHandle, channels> audio,
                                     midi::shared_vector<midi::AnyMidiEvent> midi,
                                     long nframes) noexcept
    : audio(audio[0]), midi(midi), nframes(nframes)
  {}

  inline ProcessData<1>::ProcessData(std::array<AudioBufferHandle, channels> audio,
                                     midi::shared_vector<midi::AnyMidiEvent> midi) noexcept
    : audio(audio[0]), midi(midi), nframes(audio[0].size())
  {}

  inline ProcessData<1>::ProcessData(std::array<AudioBufferHandle, channels> audio) noexcept
    : audio(audio[0]), midi(), nframes(audio[0].size())
  {}


  inline ProcessData<0> ProcessData<1>::midi_only()
  {
    return {midi, nframes};
  }

  inline ProcessData<1> ProcessData<1>::audio_only()
  {
    return {audio, {}, nframes};
  }

  template<std::size_t NN>
  inline ProcessData<NN> ProcessData<1>::redirect(const std::array<AudioBufferHandle, NN>& buf)
  {
    return ProcessData<NN>{buf, midi, nframes};
  }

  inline ProcessData<1> ProcessData<1>::redirect(const AudioBufferHandle& buf)
  {
    return ProcessData<1>{buf, midi, nframes};
  }

  /// Get only a slice of the audio.
  ///
  /// \param idx The index to start from
  /// \param length The number of frames to keep in the slice
  ///   If `length` is negative, `nframes - idx` will be used
  /// \requires parameter `idx` shall be in the range `[0, nframes)`, and
  /// `length` shall be in range `[0, nframes - idx]`
  inline ProcessData<1> ProcessData<1>::slice(int idx, int length)
  {
    auto res = *this;
    length = length < 0 ? nframes - idx : length;
    res.nframes = length;
    res.audio = audio.slice(idx, length);
    return res;
  }

  inline std::array<float*, 1> ProcessData<1>::raw_audio_buffers()
  {
    return {audio.data()};
  }

  // AUDIO FRAME //

  template<int N>
  auto AudioFrame<N>::begin() noexcept -> iterator
  {
    return data_.begin();
  }
  template<int N>
  auto AudioFrame<N>::end() noexcept -> iterator
  {
    return data_.end();
  }
  template<int N>
  auto AudioFrame<N>::begin() const noexcept -> const_iterator
  {
    return data_.begin();
  }
  template<int N>
  auto AudioFrame<N>::end() const noexcept -> const_iterator
  {
    return data_.end();
  }

  template<int N>
  template<typename Func, typename Enabled>
  AudioFrame<N>& AudioFrame<N>::transform_in_place(Func&& f) noexcept
  {
    util::transform(*this, begin(), std::forward<Func>(f));
    return *this;
  }

  template<int N>
  template<typename Func, typename Enabled>
  AudioFrame<N>& AudioFrame<N>::transform_in_place(AudioFrame<N> af, Func&& f) noexcept
  {
    util::transform(*this, af.begin(), begin(), std::forward<Func>(f));
    return *this;
  }

  template<int N>
  template<typename Func, typename Enabled>
  AudioFrame<N> AudioFrame<N>::transform(Func&& f) const noexcept
  {
    AudioFrame<N> res;
    util::transform(*this, res.begin(), std::forward<Func>(f));
    return res;
  }

  template<int N>
  template<typename Func, typename Enabled>
  AudioFrame<N> AudioFrame<N>::transform(AudioFrame<N> af, Func&& f) const noexcept
  {
    AudioFrame<N> res;
    util::transform(*this, af.begin(), res.begin(), std::forward<Func>(f));
    return res;
  }

  template<int N>
  float& AudioFrame<N>::operator[](int i) noexcept
  {
    assert(i < N);
    return data_[i];
  }

  template<int N>
  const float& AudioFrame<N>::operator[](int i) const noexcept
  {
    assert(i < N);
    return data_[i];
  }

  template<int N>
  AudioFrame<N> AudioFrame<N>::operator+(float f) const noexcept
  {
    return transform([f](float ff) { return ff + f; });
  }
  template<int N>
  AudioFrame<N> AudioFrame<N>::operator+(AudioFrame<N> af) const noexcept
  {
    return transform(af, std::plus());
  }
  template<int N>
  AudioFrame<N> AudioFrame<N>::operator-(float f) const noexcept
  {
    return transform([f](float ff) { return ff - f; });
  }
  template<int N>
  AudioFrame<N> AudioFrame<N>::operator-(AudioFrame<N> af) const
  {
    return transform(af, std::minus());
  }
  template<int N>
  AudioFrame<N> AudioFrame<N>::operator*(float f) const noexcept
  {
    return transform([f](float ff) { return ff * f; });
  }
  template<int N>
  AudioFrame<N> AudioFrame<N>::operator*(AudioFrame<N> af) const noexcept
  {
    return transform(af, std::multiplies());
  }
  template<int N>
  AudioFrame<N> AudioFrame<N>::operator/(float f) const noexcept
  {
    return transform([f](float ff) { return ff / f; });
  }
  template<int N>
  AudioFrame<N> AudioFrame<N>::operator/(AudioFrame<N> af) const noexcept
  {
    return transform(af, std::divides());
  }

  template<int N>
  AudioFrame<N>& AudioFrame<N>::operator+=(float f) noexcept
  {
    return transform_in_place([f](float ff) { return ff + f; });
  }
  template<int N>
  AudioFrame<N>& AudioFrame<N>::operator+=(AudioFrame<N> af) noexcept
  {
    return transform_in_place(af, std::plus());
  }
  template<int N>
  AudioFrame<N>& AudioFrame<N>::operator-=(float f) noexcept
  {
    return transform_in_place([f](float ff) { return ff - f; });
  }
  template<int N>
  AudioFrame<N>& AudioFrame<N>::operator-=(AudioFrame<N> af) noexcept
  {
    return transform_in_place(af, std::minus());
  }
  template<int N>
  AudioFrame<N>& AudioFrame<N>::operator*=(float f) noexcept
  {
    return transform_in_place([f](float ff) { return ff * f; });
  }
  template<int N>
  AudioFrame<N>& AudioFrame<N>::operator*=(AudioFrame<N> af) noexcept
  {
    return transform_in_place(af, std::multiplies());
  }
  template<int N>
  AudioFrame<N>& AudioFrame<N>::operator/=(float f) noexcept
  {
    return transform_in_place([f](float ff) { return ff / f; });
  }
  template<int N>
  AudioFrame<N>& AudioFrame<N>::operator/=(AudioFrame<N> af) noexcept
  {
    return transform_in_place(af, std::divides());
  }

  template<int N>
  bool AudioFrame<N>::operator==(const AudioFrame<N>& af) const noexcept
  {
    return util::equal(*this, af);
  }
  template<int N>
  bool AudioFrame<N>::operator!=(const AudioFrame<N>& af) const noexcept
  {
    return !(*this == af);
  }
  template<int N>
  bool AudioFrame<N>::operator<(const AudioFrame<N>& af) const noexcept
  {
    return util::lexicographical_compare(*this, af);
  }
  template<int N>
  bool AudioFrame<N>::operator>(const AudioFrame<N>& af) const noexcept
  {
    return util::lexicographical_compare(af, *this);
  }
  template<int N>
  bool AudioFrame<N>::operator<=(const AudioFrame<N>& af) const noexcept
  {
    return !(*this > af);
  }
  template<int N>
  bool AudioFrame<N>::operator>=(const AudioFrame<N>& af) const noexcept
  {
    return !(*this < af);
  }

  // Non-member operators

  template<int N>
  AudioFrame<N> operator+(float f, const AudioFrame<N>& af) noexcept
  {
    return af.transform([f](float ff) { return f + ff; });
  }
  template<int N>
  AudioFrame<N> operator-(float f, const AudioFrame<N>& af) noexcept
  {
    return af.transform([f](float ff) { return f - ff; });
  }
  template<int N>
  AudioFrame<N> operator*(float f, const AudioFrame<N>& af) noexcept
  {
    return af.transform([f](float ff) { return f * ff; });
  }
  template<int N>
  AudioFrame<N> operator/(float f, const AudioFrame<N>& af) noexcept
  {
    return af.transform([f](float ff) { return f / ff; });
  }
  // AudioFrameRef //

  template<int N>
  auto AudioFrameRef<N>::begin() noexcept -> iterator
  {
    return data_.begin();
  }
  template<int N>
  auto AudioFrameRef<N>::end() noexcept -> iterator
  {
    return data_.end();
  }
  template<int N>
  auto AudioFrameRef<N>::begin() const noexcept -> const_iterator
  {
    return data_.begin();
  }
  template<int N>
  auto AudioFrameRef<N>::end() const noexcept -> const_iterator
  {
    return data_.end();
  }

  template<int N>
  template<typename Func, typename Enabled>
  AudioFrameRef<N>& AudioFrameRef<N>::transform_in_place(Func&& f) noexcept
  {
    util::transform(*this, begin(), std::forward<Func>(f));
    return *this;
  }

  template<int N>
  template<typename Func, typename Enabled>
  AudioFrameRef<N>& AudioFrameRef<N>::transform_in_place(AudioFrame<N> af, Func&& f) noexcept
  {
    util::transform(*this, af.begin(), begin(), std::forward<Func>(f));
    return *this;
  }

  template<int N>
  template<typename Func, typename Enabled>
  AudioFrame<N> AudioFrameRef<N>::transform(Func&& f) const noexcept
  {
    AudioFrame<N> res;
    util::transform(*this, res.begin(), std::forward<Func>(f));
    return res;
  }

  template<int N>
  template<typename Func, typename Enabled>
  AudioFrame<N> AudioFrameRef<N>::transform(AudioFrame<N> af, Func&& f) const noexcept
  {
    AudioFrame<N> res;
    util::transform(*this, af.begin(), res.begin(), std::forward<Func>(f));
    return res;
  }

  template<int N>
  float& AudioFrameRef<N>::operator[](int i) noexcept
  {
    assert(i < N);
    return *data_[i];
  }

  template<int N>
  const float& AudioFrameRef<N>::operator[](int i) const noexcept
  {
    assert(i < N);
    return *data_[i];
  }

  template<int N>
  AudioFrame<N> AudioFrameRef<N>::operator+(float f) const noexcept
  {
    return transform([f](float ff) { return ff + f; });
  }
  template<int N>
  AudioFrame<N> AudioFrameRef<N>::operator+(AudioFrame<N> af) const noexcept
  {
    return transform(af, std::plus());
  }
  template<int N>
  AudioFrame<N> AudioFrameRef<N>::operator-(float f) const noexcept
  {
    return transform([f](float ff) { return ff - f; });
  }
  template<int N>
  AudioFrame<N> AudioFrameRef<N>::operator-(AudioFrame<N> af) const
  {
    return transform(af, std::minus());
  }
  template<int N>
  AudioFrame<N> AudioFrameRef<N>::operator*(float f) const noexcept
  {
    return transform([f](float ff) { return ff * f; });
  }
  template<int N>
  AudioFrame<N> AudioFrameRef<N>::operator*(AudioFrame<N> af) const noexcept
  {
    return transform(af, std::multiplies());
  }
  template<int N>
  AudioFrame<N> AudioFrameRef<N>::operator/(float f) const noexcept
  {
    return transform([f](float ff) { return ff / f; });
  }
  template<int N>
  AudioFrame<N> AudioFrameRef<N>::operator/(AudioFrame<N> af) const noexcept
  {
    return transform(af, std::divides());
  }

  template<int N>
  AudioFrameRef<N>& AudioFrameRef<N>::operator+=(float f) noexcept
  {
    return transform_in_place([f](float ff) { return ff + f; });
  }
  template<int N>
  AudioFrameRef<N>& AudioFrameRef<N>::operator+=(AudioFrame<N> af) noexcept
  {
    return transform_in_place(af, std::plus());
  }
  template<int N>
  AudioFrameRef<N>& AudioFrameRef<N>::operator-=(float f) noexcept
  {
    return transform_in_place([f](float ff) { return ff - f; });
  }
  template<int N>
  AudioFrameRef<N>& AudioFrameRef<N>::operator-=(AudioFrame<N> af) noexcept
  {
    return transform_in_place(af, std::minus());
  }
  template<int N>
  AudioFrameRef<N>& AudioFrameRef<N>::operator*=(float f) noexcept
  {
    return transform_in_place([f](float ff) { return ff * f; });
  }
  template<int N>
  AudioFrameRef<N>& AudioFrameRef<N>::operator*=(AudioFrame<N> af) noexcept
  {
    return transform_in_place(af, std::multiplies());
  }
  template<int N>
  AudioFrameRef<N>& AudioFrameRef<N>::operator/=(float f) noexcept
  {
    return transform_in_place([f](float ff) { return ff / f; });
  }
  template<int N>
  AudioFrameRef<N>& AudioFrameRef<N>::operator/=(AudioFrame<N> af) noexcept
  {
    return transform_in_place(af, std::divides());
  }

  template<int N>
  bool AudioFrameRef<N>::operator==(const AudioFrame<N>& af) const noexcept
  {
    return util::equal(*this, af);
  }
  template<int N>
  bool AudioFrameRef<N>::operator!=(const AudioFrame<N>& af) const noexcept
  {
    return !(*this == af);
  }
  template<int N>
  bool AudioFrameRef<N>::operator<(const AudioFrame<N>& af) const noexcept
  {
    return util::lexicographical_compare(*this, af);
  }
  template<int N>
  bool AudioFrameRef<N>::operator>(const AudioFrame<N>& af) const noexcept
  {
    return util::lexicographical_compare(af, *this);
  }
  template<int N>
  bool AudioFrameRef<N>::operator<=(const AudioFrame<N>& af) const noexcept
  {
    return !(*this > af);
  }
  template<int N>
  bool AudioFrameRef<N>::operator>=(const AudioFrame<N>& af) const noexcept
  {
    return !(*this < af);
  }

  // Non-member operators

  template<int N>
  AudioFrame<N> operator+(float f, const AudioFrameRef<N>& af) noexcept
  {
    return af.transform([f](float ff) { return f + ff; });
  }
  template<int N>
  AudioFrame<N> operator-(float f, const AudioFrameRef<N>& af) noexcept
  {
    return af.transform([f](float ff) { return f - ff; });
  }
  template<int N>
  AudioFrame<N> operator*(float f, const AudioFrameRef<N>& af) noexcept
  {
    return af.transform([f](float ff) { return f * ff; });
  }
  template<int N>
  AudioFrame<N> operator/(float f, const AudioFrameRef<N>& af) noexcept
  {
    return af.transform([f](float ff) { return f / ff; });
  }

  namespace detail {
    template<typename... Refs>
    AudioFrameRef<sizeof...(Refs)> zip_audio_transform(Refs&... refs)
    {
      static_assert((std::is_convertible_v<std::decay_t<Refs>, float> && ...),
                    "zip_audio requires a range that is convertible to floats");
      return AudioFrameRef<sizeof...(Refs)>({&static_cast<float&>(refs)...});
    }

    template<typename... Refs>
    AudioFrame<sizeof...(Refs)> zip_audio_transform(const Refs&... refs)
    {
      static_assert((std::is_convertible_v<std::decay_t<Refs>, float> && ...),
                    "zip_audio requires a range that is convertible to floats");
      return AudioFrame<sizeof...(Refs)>({static_cast<float>(refs)...});
    }
  } // namespace detail

  template<typename... ContainerRefs>
  auto zip_audio(ContainerRefs&&... crfs) noexcept
  {
    return util::view::transform(
      util::zip(std::forward<ContainerRefs>(crfs)...),  //
      [](auto&& tuple_of_refs) {
        return std::apply(
          [](auto&&... refs) {
            return detail::zip_audio_transform(std::forward<decltype(refs)>(refs)...);
          },
          tuple_of_refs);
      });
  }


} // namespace otto::core::audio
