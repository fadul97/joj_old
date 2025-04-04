#ifndef _JOJ_WIN32_WINDOW_FACTORY_H
#define _JOJ_WIN32_WINDOW_FACTORY_H

#include "joj/core/defines.h"

#if JOJ_PLATFORM_WINDOWS

#include "joj/platform/window_factory.h"
#include "joj/platform/win32/window_win32.h"

namespace joj
{
    /*
    wnd_class.cbSize = sizeof(WNDCLASSEX);
    wnd_class.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW | WS_CHILD;
    wnd_class.lpfnWndProc = GUIWinProc;
    wnd_class.cbClsExtra = 0;
    wnd_class.cbWndExtra = 0;
    wnd_class.hInstance = app_id;
    wnd_class.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wnd_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wnd_class.hbrBackground = CreateSolidBrush(RGB(60, 60, 60));
    wnd_class.lpszMenuName = nullptr;
    wnd_class.lpszClassName = gui_class_name;
    wnd_class.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    */

    class JOJ_API Win32WindowFactory : public WindowFactory<Win32Window>
    {
    public:
        Win32WindowFactory();
        ~Win32WindowFactory();

        Win32Window* create_window(const char* title, const u32 width,
            const u32 height, const WindowMode mode) override;
        ErrorCode create_window_class(WindowRegistrationClass& window_class) override;

    private:
        DWORD convert_window_styles(WindowStyles styles);
    };
}

#endif // JOJ_PLATFORM_WINDOWS

#endif // _JOJ_WIN32_WINDOW_FACTORY_H