#pragma once

#include <array>

#include "util/view.hpp"

namespace otto::core::audio {

  template<int N>
  struct AudioFrame {
    using iterator = typename std::array<float, N>::iterator;
    using const_iterator = typename std::array<float, N>::const_iterator;

    static constexpr int channels = N;

    AudioFrame() noexcept = default;
    AudioFrame(std::array<float, N> il) : data_(std::move(il)) {}

    template<typename Func, typename = std::enable_if_t<util::is_invocable_r_v<float, Func, float>>>
    AudioFrame<N>& transform_in_place(Func&& f) noexcept;
    template<typename Func,
             typename = std::enable_if_t<util::is_invocable_r_v<float, Func, float, float>>>
    AudioFrame<N>& transform_in_place(AudioFrame<N> af, Func&& f) noexcept;

    template<typename Func, typename = std::enable_if_t<util::is_invocable_r_v<float, Func, float>>>
    AudioFrame<N> transform(Func&& f) const noexcept;
    template<typename Func,
             typename = std::enable_if_t<util::is_invocable_r_v<float, Func, float, float>>>
    AudioFrame<N> transform(AudioFrame<N> af, Func&& f) const noexcept;

    // iterators
    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;

    // accessors

    float& operator[](int i) noexcept;
    const float& operator[](int i) const noexcept;

    AudioFrame<N> operator+(float f) const noexcept;
    AudioFrame<N> operator+(AudioFrame<N> af) const noexcept;
    AudioFrame<N> operator-(float f) const noexcept;
    AudioFrame<N> operator-(AudioFrame<N> af) const;
    AudioFrame<N> operator*(float f) const noexcept;
    AudioFrame<N> operator*(AudioFrame<N> af) const noexcept;
    AudioFrame<N> operator/(float f) const noexcept;
    AudioFrame<N> operator/(AudioFrame<N> af) const noexcept;

    friend AudioFrame<N> operator+(float f, const AudioFrame<N>&) noexcept;
    friend AudioFrame<N> operator-(float f, const AudioFrame<N>&) noexcept;
    friend AudioFrame<N> operator*(float f, const AudioFrame<N>&) noexcept;
    friend AudioFrame<N> operator/(float f, const AudioFrame<N>&) noexcept;

    AudioFrame<N>& operator+=(float f) noexcept;
    AudioFrame<N>& operator+=(AudioFrame<N> af) noexcept;
    AudioFrame<N>& operator-=(float f) noexcept;
    AudioFrame<N>& operator-=(AudioFrame<N> af) noexcept;
    AudioFrame<N>& operator*=(float f) noexcept;
    AudioFrame<N>& operator*=(AudioFrame<N> af) noexcept;
    AudioFrame<N>& operator/=(float f) noexcept;
    AudioFrame<N>& operator/=(AudioFrame<N> af) noexcept;

    bool operator==(const AudioFrame<N>& af) const noexcept;
    bool operator!=(const AudioFrame<N>& af) const noexcept;
    bool operator<(const AudioFrame<N>& af) const noexcept;
    bool operator>(const AudioFrame<N>& af) const noexcept;
    bool operator<=(const AudioFrame<N>& af) const noexcept;
    bool operator>=(const AudioFrame<N>& af) const noexcept;

  private:
    std::array<float, channels> data_;
  };

  template<int N>
  AudioFrame(std::array<float, N>)->AudioFrame<N>;

  template<int N>
  struct AudioFrameRef {
    using iterator = util::double_dereference_iterator<typename std::array<float*, N>::iterator>;
    using const_iterator =
      util::double_dereference_iterator<typename std::array<float*, N>::const_iterator>;

    static constexpr int channels = N;

    AudioFrameRef(std::array<float*, N> il) : data_(std::move(il)) {}

    template<typename Func, typename = std::enable_if_t<util::is_invocable_r_v<float, Func, float>>>
    AudioFrameRef<N>& transform_in_place(Func&& f) noexcept;
    template<typename Func,
             typename = std::enable_if_t<util::is_invocable_r_v<float, Func, float, float>>>
    AudioFrameRef<N>& transform_in_place(AudioFrame<N> af, Func&& f) noexcept;

    template<typename Func, typename = std::enable_if_t<util::is_invocable_r_v<float, Func, float>>>
    AudioFrame<N> transform(Func&& f) const noexcept;
    template<typename Func,
             typename = std::enable_if_t<util::is_invocable_r_v<float, Func, float, float>>>
    AudioFrame<N> transform(AudioFrame<N> af, Func&& f) const noexcept;

    // iterators
    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;

    // accessors

    float& operator[](int i) noexcept;
    const float& operator[](int i) const noexcept;

    AudioFrame<N> operator+(float f) const noexcept;
    AudioFrame<N> operator+(AudioFrame<N> af) const noexcept;
    AudioFrame<N> operator-(float f) const noexcept;
    AudioFrame<N> operator-(AudioFrame<N> af) const;
    AudioFrame<N> operator*(float f) const noexcept;
    AudioFrame<N> operator*(AudioFrame<N> af) const noexcept;
    AudioFrame<N> operator/(float f) const noexcept;
    AudioFrame<N> operator/(AudioFrame<N> af) const noexcept;

    friend AudioFrame<N> operator+(float f, const AudioFrameRef<N>&) noexcept;
    friend AudioFrame<N> operator-(float f, const AudioFrameRef<N>&) noexcept;
    friend AudioFrame<N> operator*(float f, const AudioFrameRef<N>&) noexcept;
    friend AudioFrame<N> operator/(float f, const AudioFrameRef<N>&) noexcept;

    AudioFrameRef<N>& operator+=(float f) noexcept;
    AudioFrameRef<N>& operator+=(AudioFrame<N> af) noexcept;
    AudioFrameRef<N>& operator-=(float f) noexcept;
    AudioFrameRef<N>& operator-=(AudioFrame<N> af) noexcept;
    AudioFrameRef<N>& operator*=(float f) noexcept;
    AudioFrameRef<N>& operator*=(AudioFrame<N> af) noexcept;
    AudioFrameRef<N>& operator/=(float f) noexcept;
    AudioFrameRef<N>& operator/=(AudioFrame<N> af) noexcept;

    bool operator==(const AudioFrame<N>& af) const noexcept;
    bool operator!=(const AudioFrame<N>& af) const noexcept;
    bool operator<(const AudioFrame<N>& af) const noexcept;
    bool operator>(const AudioFrame<N>& af) const noexcept;
    bool operator<=(const AudioFrame<N>& af) const noexcept;
    bool operator>=(const AudioFrame<N>& af) const noexcept;

    operator AudioFrame<N>() const noexcept;

  private:
    std::array<float*, channels> data_;
  };

  template<int N>
  AudioFrameRef(std::array<float*, N>)->AudioFrameRef<N>;

  template<typename... Args>
  AudioFrame<sizeof...(Args)> frame(Args... args) noexcept
  {
    return AudioFrame<sizeof...(Args)>({static_cast<float>(args)...});
  }

  template<typename... Args>
  AudioFrame<sizeof...(Args)> frame_ref(Args... args) noexcept
  {
    return AudioFrameRef<sizeof...(Args)>({&static_cast<float&>(args)...});
  }

  /// Zip audio buffers (any containers of floats).
  ///
  /// The resulting range contains {@ref AudioFrame}s.
  template<typename... ContainerRefs>
  auto zip_audio(ContainerRefs&&... crfs) noexcept;

} // namespace otto::core::audio

#include "audio_frame.inl"
