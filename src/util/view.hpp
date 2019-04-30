#pragma once

#include <tl/optional.hpp>
#include "util/algorithm.hpp"
#include "util/iterator.hpp"

namespace otto::util {

  template<typename BeginIter, typename EndIter>
  struct View;

  struct SentinelValue {};

  template<typename ValueType = SentinelValue>
  struct SentinelIterator
    : iterator_facade<SentinelIterator<ValueType>, ValueType, std::bidirectional_iterator_tag> {
    constexpr SentinelIterator() = default;
    constexpr void advance() {}
    constexpr ValueType dereference()
    {
      assert(false && "dereferencing sentinel iterator");
    }
    constexpr bool equal(const SentinelIterator& o) const
    {
      return true;
    }

    template<typename O>
    constexpr bool operator==(O&& o) const noexcept
    {
      return std::is_same_v<O, SentinelIterator<ValueType>>;
    }

    template<typename O>
    constexpr bool operator!=(O&& o) const noexcept
    {
      return !std::is_same_v<O, SentinelIterator<ValueType>>;
    }
  };

  template<typename ValueType = SentinelValue>
  constexpr inline SentinelIterator<ValueType> nulliter = {};

  constexpr auto index_view(int start_val = 0);

  /// A lazily evaluated view of a range.
  ///
  /// Provides operations to construct new views, and collect the data.
  template<typename BeginIter, typename EndIter>
  struct View {
    using value_type = typename iterator::detail::template value_type_t<BeginIter>;
    using reference = typename iterator::detail::template reference_t<BeginIter>;

    template<typename ContRef>
    constexpr View(ContRef&& cont) : first_(std::begin(cont)), last_(std::end(cont))
    {}
    constexpr View(BeginIter first, EndIter last) : first_(first), last_(last) {}

    constexpr BeginIter begin() const;

    constexpr EndIter end() const;

    constexpr auto rbegin() const;

    constexpr auto rend() const;

    constexpr bool empty() const;

    template<typename S = value_type, typename BinOp = std::plus<value_type>>
    constexpr auto accumulate(S&& init = {}, BinOp&& op = {}) const;

    template<typename FuncRef>
    constexpr auto transform(FuncRef&& f) const;

    template<typename Predicate>
    constexpr auto filter(Predicate&& c) const;

    constexpr auto circular() const;

    constexpr auto find() const;

    template<typename Predicate>
    constexpr auto find_if(Predicate&& p) const;

    std::vector<value_type> collect() const;

    template<typename Cont>
    constexpr Cont collect() const;

    constexpr auto reverse() const;

    template<typename Pred>
    constexpr bool for_one(Pred&& p) const;

    template<typename Pred>
    constexpr bool for_all(Pred&& p) const;

    template<typename Pred>
    constexpr bool for_none(Pred&& p) const;

    template<typename Range>
    constexpr auto zip(Range&& rng) const;

    constexpr auto indexed(int start_val = 0) const;

  private:
    BeginIter first_;
    EndIter last_;
  };

  template<typename ContRef>
  View(ContRef&& cont)
    ->View<std::decay_t<decltype(std::begin(cont))>, std::decay_t<decltype(std::end(cont))>>;

} // namespace otto::util

#include "view.inl"
