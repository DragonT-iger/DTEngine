#pragma once
#include <exception>
#include <cstdio>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>

class DXHelper
{
public:
    class com_exception : public std::exception
    {
    public:
        explicit com_exception(HRESULT hr) noexcept : result(hr) {}

        const char* what() const noexcept override
        {
            static char s_str[64];
            sprintf_s(s_str, "Failure with HRESULT of 0x%08X",
                static_cast<unsigned int>(result));
            return s_str;
        }

        HRESULT Result() const noexcept { return result; }

    private:
        HRESULT result;
    };

public:
    static inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
            throw com_exception(hr);
    }

    static inline const char* HrToString(HRESULT hr)
    {
        static char buf[64];
        sprintf_s(buf, "0x%08X", static_cast<unsigned int>(hr));
        return buf;
    }
};
