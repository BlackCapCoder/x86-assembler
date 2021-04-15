#include <iostream>
#include <tuple>

using namespace std;


template <auto F>
struct Comma_Impl
{
  // Lambda->*function boilerplate
  template <class>
  struct Lamb;

  template <class R, class C, class...As>
  struct Lamb <R (C::*) (As...) const> { using T = R (*) (As...); };

  template
    < class   = tuple    <>
    , class   = decltype ([]
    < class f = decltype (F)>()
      { if constexpr (is_pointer<f>::value)
          return F;
        else
          return typename Lamb<decltype(&f::operator())>::T {};
      }()) >
  struct Impl;

  // Curry
  template <class...Have, class R, class X, class...Xs>
  struct Impl<tuple<Have...>, R(*)(X, Xs...)> {
    const tuple <Have...> t;

    constexpr inline auto operator , (X x) {
      return [] (tuple<Have..., X> t) {
        if constexpr (sizeof...(Xs) == 0)
          return apply (F, t);
        else
          return Impl <decltype(t), R(*)(Xs...)> { t };
      } (tuple_cat (t, forward_as_tuple(x)));
    }
  };
};

template <auto F>
constexpr typename Comma_Impl<F>::template Impl<> Comma {};


void printInts (int a, int b, int c)
{
  cout << a << ", " << b << ", " << c << endl;
}


int main ()
{
  auto x = Comma <printInts>;
  auto y = Comma <[](int a, int b){cout << a << ", " << b << endl;}>;

  x, 1, 2, 3;
  y, 4, 5;

  return 0;
}
