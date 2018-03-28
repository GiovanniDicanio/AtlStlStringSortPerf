///////////////////////////////////////////////////////////////////////////////
//
//          *** ATL vs. STL String Sorting Performance Tests ***
//
//                          by Giovanni Dicanio
//
//     Compares sorting times of vector<CStringW> vs. vector<wstring>
// 
///////////////////////////////////////////////////////////////////////////////

// Define the following macro to enable custom std::wstring comparison 
// using wcscmp(), instead of the default wmemcmp()
//#define TEST_STL_WCSCMP_COMPARE


//=============================================================================
//                                  Includes
//=============================================================================

#include <string.h>     // wcscmp

#include <algorithm>    // std::shuffle, std::sort
#include <exception>    // std::exception
#include <iostream>     // std::cout
#include <random>       // std::mt19937
#include <string>       // std::wstring, std::string
#include <vector>       // std::vector

#include <Windows.h>    // Windows Platform SDK
#include <atlexcept.h>  // CAtlException
#include <atlstr.h>     // ATL CStringW

using namespace std;
using namespace ATL;


//=============================================================================
//                        Performance Counter Helpers
//=============================================================================

long long Counter() 
{
    LARGE_INTEGER li;
    ::QueryPerformanceCounter(&li);
    return li.QuadPart;
}

long long Frequency() 
{
    LARGE_INTEGER li;
    ::QueryPerformanceFrequency(&li);
    return li.QuadPart;
}

void PrintTime(const long long start, const long long finish, const char * const s) 
{
    cout << s << ": " << (finish - start) * 1000.0 / Frequency() << " ms \n";
}


//=============================================================================
//                           Performance Tests
//=============================================================================


#ifdef TEST_STL_WCSCMP_COMPARE
bool CompareUsingWcscmp(const std::wstring& a, const std::wstring& b)
{
    // a < b
    return (wcscmp(a.c_str(), b.c_str())) < 0;
}
#endif // TEST_STL_WCSCMP_COMPARE


void RunTests()
{
    cout << "\n*** String Sorting Performance Tests -- by Giovanni Dicanio *** \n\n";

#ifdef _WIN64
    cout << "[64-bit build] \n\n";
#else
    cout << "[32-bit build] \n\n";
#endif

    // Build a vector of strings generated starting from "Lorem Ipsum"
    const auto shuffled = []() -> vector<wstring> 
    {
        const wstring lorem[] = 
        {
            L"Lorem ipsum dolor sit amet, consectetuer adipiscing elit.",
            L"Maecenas porttitor congue massa. Fusce posuere, magna sed",
            L"pulvinar ultricies, purus lectus malesuada libero,",
            L"sit amet commodo magna eros quis urna.",
            L"Nunc viverra imperdiet enim. Fusce est. Vivamus a tellus.",
            L"Pellentesque habitant morbi tristique senectus et netus et",
            L"malesuada fames ac turpis egestas. Proin pharetra nonummy pede.",
            L"Mauris et orci. [*** add more chars to prevent SSO ***]"
        };

        vector<wstring> v;
#ifdef _DEBUG
        constexpr int kTestIterationCount = 1000;
#else
        constexpr int kTestIterationCount = 200'000;
#endif
        for (int i = 0; i < kTestIterationCount; ++i) 
        {
            for (const auto & s : lorem) 
            {
                v.push_back(s + L" (#" + to_wstring(i) + L")");
            }

        }

        mt19937 prng(1980);
        shuffle(v.begin(), v.end(), prng);
        return v;
    }();

    const auto shuffledPtrs = [&]() -> vector<const wchar_t *> 
    {
        vector<const wchar_t *> v;
        for (const auto& s : shuffled) 
        {
            v.push_back(s.c_str());
        }
        return v;
    }();

    cout << "Test string array contains " << (shuffled.size() / 1000) << "K strings. \n\n";
    
    long long start = 0;
    long long finish = 0;

    vector<CStringW> atl1(shuffledPtrs.begin(), shuffledPtrs.end());
    vector<wstring>  stl1 = shuffled;
    vector<CStringW> atl2(shuffledPtrs.begin(), shuffledPtrs.end());
    vector<wstring>  stl2 = shuffled;
    vector<CStringW> atl3(shuffledPtrs.begin(), shuffledPtrs.end());
    vector<wstring>  stl3 = shuffled;

    //-------------------------------------------------------------------------

    cout << "=== String Sorting Test === \n";

#ifdef TEST_STL_WCSCMP_COMPARE
    cout << "*** Comparing std::wstrings using wcscmp ***\n";
#endif

    start = Counter();
    sort(atl1.begin(), atl1.end());
    finish = Counter();
    PrintTime(start, finish, "ATL1");

    start = Counter();
#ifdef TEST_STL_WCSCMP_COMPARE
    sort(stl1.begin(), stl1.end(), CompareUsingWcscmp);
#else
    sort(stl1.begin(), stl1.end());
#endif
    finish = Counter();
    PrintTime(start, finish, "STL1");

    start = Counter();
    sort(atl2.begin(), atl2.end());
    finish = Counter();
    PrintTime(start, finish, "ATL2");

    start = Counter();
#ifdef TEST_STL_WCSCMP_COMPARE
    sort(stl2.begin(), stl2.end(), CompareUsingWcscmp);
#else
    sort(stl2.begin(), stl2.end());
#endif    
    finish = Counter();
    PrintTime(start, finish, "STL2");

    start = Counter();
    sort(atl3.begin(), atl3.end());
    finish = Counter();
    PrintTime(start, finish, "ATL3");

    start = Counter();
#ifdef TEST_STL_WCSCMP_COMPARE
    sort(stl3.begin(), stl3.end(), CompareUsingWcscmp);
#else
    sort(stl3.begin(), stl3.end());
#endif    
    finish = Counter();
    PrintTime(start, finish, "STL3");

    cout << '\n';
}


int main()
{
    constexpr int kExitOk = 0;
    constexpr int kExitError = 1;

    try
    {
        RunTests();
    }
    catch (const CAtlException& e)
    {
        cerr << "\n*** ERROR: CAtlException thrown; HR=0x" << hex << e.m_hr << '\n';
        return kExitError;
    }
    catch (const exception& e)
    {
        cerr << "\n*** ERROR: " << e.what() << '\n';
        return kExitError;
    }

    return kExitOk;
}

///////////////////////////////////////////////////////////////////////////////
