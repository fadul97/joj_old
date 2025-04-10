#ifndef _JOJ_WINDOW_HANDLE_H
#define _JOJ_WINDOW_HANDLE_H

#include "joj/core/defines.h"

namespace joj
{
    template<typename T>
    class JOJ_API WindowHandle
    {
    public:
        WindowHandle();
        virtual ~WindowHandle() = 0;

        T& get() const;

    private:
        T m_handle;
    };

    WindowHandle::WindowHandle() {}

    T& WindowHandle::get() const
    {
        return m_handle;
    }
}

#endif // _JOJ_WINDOW_HANDLE_H