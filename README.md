# AtlStlStringSortPerf
Comparing string vector sorting using ATL::CStringW vs. std::wstring

by Giovanni Dicanio

I wrote [this code](../master/AtlStlStringSortPerf/AtlStlStringSortPerf.cpp) to compare sorting performance of ATL strings (`ATL::CStringW`) vs. C++ Standard Library strings (`std::wstring`).

The benchmark consists in building `std::vector`s of shuffled strings (`vector<CStringW>` and `vector<wstring>`), and invoking `std::sort()` on them, measuring the sorting times.

Building the code using Visual Studio 2015 with Update 3, I got the following results (Win10 PC, Intel Core i7 CPU @3.4GHz, 32GB RAM):

```
*** String Sorting Performance Tests -- by Giovanni Dicanio ***

[64-bit build]

Test string array contains 1600K strings.

=== String Sorting Test ===
ATL1: 2926.5 ms
STL1: 3319.12 ms
ATL2: 2936.48 ms
STL2: 3287.89 ms
ATL3: 2919.14 ms
STL3: 3257.14 ms
```

These shows that `CStringW` performs better than `wstring` in this context.

I was curious _why_ sorting `std::wstring`s was slower than `CStringW`s, so I delved into the implementation code of both classes. Besides the ATL string's Copy-on-Write policy, another difference I noted was in the way strings are _compared_ (and comparing strings is a key operation when sorting a string vector). In particular, `CStringW` uses **`wcscmp()`** (see the implementation code of `ChTraitsCRT<wchar_t>::StringCompare()` in the `<cstringt.h>` header). On the other hand, `std::wstring` uses **`wmemcmp()`** (see `char_traits<wchar_t>::compare()` in `<iosfwd>`).

Out of curiosity, I tried invoking `std::sort()` on the `vector<wstring>`, passing a custom comparison function that overrides the default `wstring` implementation and calls `wcscmp()` instead. You can enable that in [the benchmark code](../master/AtlStlStringSortPerf/AtlStlStringSortPerf.cpp) defining the **`TEST_STL_WCSCMP_COMPARE`** macro.  
As a result of that, sorting `std::wstring`s _outperforms_ `ATL::CStringW`:

```
*** String Sorting Performance Tests -- by Giovanni Dicanio ***

[64-bit build]

Test string array contains 1600K strings.

=== String Sorting Test ===
*** Comparing std::wstrings using wcscmp ***
ATL1: 2902.56 ms
STL1: 2376.93 ms
ATL2: 2895.24 ms
STL2: 2348.83 ms
ATL3: 2889.57 ms
STL3: 2308.84 ms
```
