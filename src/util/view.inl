#pragma once

namespace otto::util {

  // COLLECTORS //

  template<typename T>
  struct collector<std::vector<T>> {
    template<typename BIter, typename EIter>
    static std::vector<T> collect(BIter b, EIter e)
    {
      return std::vector<T>{b, e};
    }
  };

  template<typename T, std::size_t N>
  struct collector<std::array<T, N>> {
    template<typename BIter, typename EIter>
    static std::array<T, N> collect(BIter b, EIter e)
    {
      std::array<T, N> res;
      int i = 0;
      for (; i < N && b != e; b++, i++) {
        res[i] = *b;
      }
      // TODO: Throw exception instead? or do other error handling?
      assert(i == N);
      assert(b == e);
      return res;
    }
  };

  // VIEW //

  template<typename BeginIter, typename EndIter>
  constexpr auto make_view(BeginIter&& b, EndIter&& e)
  {
    return View(std::forward<BeginIter>(b), std::forward<EndIter>(e));
  }

  template<typename Range>
  constexpr auto make_view(Range&& rng)
  {
    return View(std::forward<Range>(rng));
  }

  constexpr auto index_view(int start_val)
  {
    return View(generator([n = start_val]() mutable { return n++; }), nulliter<>);
  }

  template<typename Func>
  constexpr auto generate(Func&& f)
  {
    return View(generator(std::forward<f>()), nulliter<>);
  }

  template<typename B, typename E>
  constexpr B View<B, E>::begin() const
  {
    return first_;
  }

  template<typename B, typename E>
  constexpr E View<B, E>::end() const
  {
    return last_;
  }

  template<typename B, typename E>
  constexpr auto View<B, E>::rbegin() const
  {
    return std::make_reverse_iterator(first_);
  }

  template<typename B, typename E>
  constexpr auto View<B, E>::rend() const
  {
    return std::make_reverse_iterator(last_);
  }

  template<typename B, typename E>
  constexpr bool View<B, E>::empty() const
  {
    return first_ == last_;
  }

  template<typename B, typename E>
  template<typename S, typename BinOp>
  constexpr auto View<B, E>::accumulate(S&& init, BinOp&& op) const
  {
    auto res = std::forward<S>(init);
    for (; first_ != last_; ++first_) res = op(res, *first_);
    return res;
  }

  template<typename B, typename E>
  template<typename FuncRef>
  constexpr auto View<B, E>::transform(FuncRef&& f) const
  {
    using transformiter = transform_iterator<B, std::decay_t<FuncRef>>;
    auto first = transformiter(begin(), std::forward<FuncRef>(f));
    auto last = transformiter(end(), first);
    return make_view(std::move(first), std::move(last));
  }

  template<typename B, typename E>
  template<typename Predicate>
  constexpr auto View<B, E>::filter(Predicate&& c) const
  {
    using filteriter = filter_iterator<decltype(begin()), std::decay_t<Predicate>>;
    auto first = filteriter(begin(), end(), std::forward<Predicate>(c));
    auto last = filteriter(end(), end(), first);
    return make_view(std::move(first), std::move(last));
  }

  template<typename B, typename E>
  constexpr auto View<B, E>::circular() const
  {
    using CircIter = circular_iterator<decltype(begin())>;
    auto first = CircIter(begin(), begin(), end());
    auto last = CircIter(end(), begin(), end());
    return make_view(first, last);
  }

  template<typename B, typename E>
  constexpr auto View<B, E>::find() const -> tl::optional<value_type>
  {
    auto found = std::find(begin(), end());
    if (found == end()) return tl::nullopt;
    return tl::make_optional(*found);
  }

  template<typename B, typename E>
  template<typename Predicate>
  constexpr auto View<B, E>::find_if(Predicate&& p) const -> tl::optional<value_type>
  {
    auto found = std::find_if(begin(), end(), std::forward<Predicate>(p));
    if (found == end()) return tl::nullopt;
    return tl::make_optional(*found);
  }

  template<typename B, typename E>
  auto View<B, E>::collect() const -> std::vector<value_type>
  {
    return std::vector<value_type>(begin(), end());
  }

  template<typename B, typename E>
  template<typename Cont>
  constexpr Cont View<B, E>::collect() const
  {
    return collector<Cont>::collect(begin(), end());
  }

  template<typename B, typename E>
  constexpr auto View<B, E>::reverse() const
  {
    return make_view(rbegin(), rend());
  }

  template<typename B, typename E>
  template<typename Pred>
  constexpr bool View<B, E>::for_one(Pred&& p) const
  {
    for (auto&& v : *this)
      if (p(v)) return true;
    return false;
  }

  template<typename B, typename E>
  template<typename Pred>
  constexpr bool View<B, E>::for_all(Pred&& p) const
  {
    for (auto&& v : *this)
      if (!p(v)) return false;
    return true;
  }

  template<typename B, typename E>
  template<typename Pred>
  constexpr bool View<B, E>::for_none(Pred&& p) const
  {
    return !for_one(std::forward<Pred>(p));
  }

  template<typename B, typename E>
  template<typename Range>
  constexpr auto View<B, E>::zip(Range&& rng) const
  {
    return make_view(zip_iters(first_, std::begin(rng)), zip_iters(last_, std::end(rng)));
  }

  template<typename B, typename E>
  constexpr auto View<B, E>::indexed(int start_val) const
  {
    return make_view(generator([n = start_val]() mutable { return n++; }), nulliter<int>)
      .zip(*this);
  }


} // namespace otto::util
