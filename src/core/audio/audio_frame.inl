#pragma once

namespace otto::core::audio {

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
    return util::zip(std::forward<ContainerRefs>(crfs)...) //
      .transform([](auto&& tuple_of_refs) {
        return std::apply(
          [](auto&&... refs) {
            return detail::zip_audio_transform(std::forward<decltype(refs)>(refs)...);
          },
          tuple_of_refs);
      });
  }

} // namespace otto::core::audio
