#include "renderer/d3d11/gui/gui_d3d11.h"

#if JPLATFORM_WINDOWS

#include "jmacros.h"
#include "logger.h"
#include "platform/win32/window_win32.h"
#include <windowsx.h>
#include "platform/win32/input_win32.h"
#include "platform/window_registration_class.h"
#include "platform/win32/window_factory_win32.h"

joj::IRenderer* joj::D3D11GUI::s_renderer = nullptr;
joj::Mouse joj::D3D11GUI::s_mouse = { 0 };
WNDPROC joj::D3D11GUI::s_originalWndProc = nullptr;

#define IDC_BUTTON1 2010

joj::D3D11GUI::D3D11GUI()
    : m_initialized(false)
{
    m_parent_window.handle = nullptr;
    m_parent_window.hdc = nullptr;
    m_parent_window.window_mode = WindowMode::Windowed;
    m_parent_window.width = 0;
    m_parent_window.height = 0;

    m_window.handle = nullptr;
    m_window.hdc = nullptr;
    m_window.window_mode = WindowMode::Windowed;
    m_window.width = 0;
    m_window.height = 0;
}

joj::D3D11GUI::~D3D11GUI()
{
}

void joj::D3D11GUI::init(WindowData& window, IRenderer& renderer)
{
    s_renderer = &renderer;

    WindowRegistrationClass wnd_class;
    wnd_class.styles =
        WindowStyles::DoubleClick | WindowStyles::OwnDC |
        WindowStyles::Horizontal | WindowStyles::Vertical;
    wnd_class.procedure = (void*)GUIWinProc;
    wnd_class.instance = GetModuleHandle(nullptr);
    wnd_class.menu_name = "";
    wnd_class.class_name = "JGUI_WINDOW_CLASS";

    joj::Win32WindowFactory factory;
    if JOJ_FAILED(factory.create_window_class(wnd_class))
    {
        JERROR(ErrorCode::ERR_WINDOW_REGISTRATION, "Failed to create window class.");
        return;
    }

    m_parent_window.handle = window.handle;
    m_parent_window.hdc = window.hdc;
    m_parent_window.window_mode = window.window_mode;
    m_parent_window.width = window.width;
    m_parent_window.height = window.height;

    HINSTANCE app_id = window.instance;

    if (m_parent_window.handle)
    {
        s_originalWndProc = (WNDPROC)SetWindowLongPtr(m_parent_window.handle, GWLP_WNDPROC, (LONG_PTR)GUIWinProc);
    }

    i32 width = m_parent_window.width / 4;
    i32 height = m_parent_window.height;

    m_window.handle = CreateWindowEx(
        0,
        wnd_class.class_name.c_str(),
        "GUI Window",
        WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE,
        0, 0,
        width, height,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!m_window.handle)
    {
        JFATAL(ErrorCode::ERR_WINDOW_HANDLE, "Failed to create window.");
        return;
    }

    ParentData parent_data = { m_window.handle, app_id };
    m_factory = JWin32WidgetFactory(parent_data);

    m_initialized = true;
}

static f32 time_elaped = 0.0f;

void joj::D3D11GUI::update(const f32 dt, const i32 xmouse, const i32 ymouse, const b8 clicked)
{
    if (m_initialized == false)
    {
        JERROR(ErrorCode::ERR_GUI_NOT_INITIALIZED, "GUI not initialized.");
        return;
    }

    time_elaped += dt;

    for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it)
    {
        auto& widget = *it;
        // JDEBUG("Clicked: %d", clicked);
        widget->update(xmouse, ymouse, clicked);
        if (widget->is_hovered(s_mouse.x, s_mouse.y) && clicked)
        {
            // JDEBUG("Mouse hovered");
        }
    }

    for (auto it = JWidget::get_widget_map().begin(); it != JWidget::get_widget_map().end(); ++it)
    {
        auto& widget = it->second;
        widget->update(xmouse, ymouse, clicked);
    }
}

void joj::D3D11GUI::draw(CommandList& cmd_list)
{
    if (m_initialized == false)
    {
        JERROR(ErrorCode::ERR_GUI_NOT_INITIALIZED, "GUI not initialized.");
        return;
    }

    for (auto& widget : m_widgets)
    {
        widget->draw(cmd_list);
    }
}

void joj::D3D11GUI::shutdown()
{
    if (m_initialized == false)
    {
        JERROR(ErrorCode::ERR_GUI_NOT_INITIALIZED, "GUI not initialized.");
        return;
    }

    for (auto& widget : m_widgets)
    {
        delete widget;
    }

    if (m_parent_window.hdc != nullptr)
        ReleaseDC(m_parent_window.handle, m_parent_window.hdc);

    if (m_parent_window.handle != nullptr)
        DestroyWindow(m_parent_window.handle);
}

void joj::D3D11GUI::add_widget(JWidget* widget)
{
    m_widgets.push_back(widget);
}

void joj::D3D11GUI::add_button(i32 x, i32 y, i32 width, i32 height,
    const std::string& label, const JEvent::Callback& callback)
{
    JButton* button = m_factory.create_button(10, 10, 100, 30, "Click Me!");
    if (!button)
    {
        JFATAL(ErrorCode::ERR_GUI_BUTTON_WIN32_CREATION, "Failed to create button.");
        return;
    }

    if (callback)
    {
        button->on_click(callback);
        button->set_callback(callback);
    }

    // m_widgets.push_back(button);
}

void joj::D3D11GUI::add_button(const std::string& label, i32 x, i32 y, i32 width, i32 height,
    const std::function<void()>& function)
{
    JButton* button = m_factory.create_button(10, 10, 100, 30, "Click Me!", function);
    if (!button)
    {
        JFATAL(ErrorCode::ERR_GUI_BUTTON_WIN32_CREATION, "Failed to create button.");
        return;
    }
}


LRESULT CALLBACK joj::D3D11GUI::GUIWinProc(HWND hWnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
    {
        i32 control_id = LOWORD(wParam); // ID do controle
        i32 notification_code = HIWORD(wParam); // C�digo de notifica��o
        HWND sender_handle = (HWND)lParam; // Handle do controle que enviou

        if (notification_code == BN_CLICKED)
        {
            // JDEBUG("Button clicked! Control ID: %d, Handle: %p", control_id, sender_handle);
            // Voc� pode procurar o widget no mapa aqui se necess�rio
            auto it = JWidget::get_widget_map().find(sender_handle);
            if (it != JWidget::get_widget_map().end())
            {
                auto widget = it->second;
                widget->trigger();
                // return widget->handle_message(msg, wParam, lParam);
                return 0;
            }
            else
            {
                JDEBUG("Widget not found in map for handle: %p", sender_handle);
            }
        }
        break;
    }
    // Read Mouse Position
    case WM_MOUSEMOVE:
    {
        s_mouse.x = GET_X_LPARAM(lParam);
        s_mouse.y = GET_Y_LPARAM(lParam);
        return 0;
    }
    // Read Mouse Button
    case WM_LBUTTONDOWN:
    {
        s_mouse.buttons[BUTTON_LEFT] = true;
        return 0;
    }
    case WM_LBUTTONUP:
    {
        s_mouse.buttons[BUTTON_LEFT] = false;
        return 0;
    }
    case WM_MBUTTONDOWN:
    {
        s_mouse.buttons[BUTTON_MIDDLE] = true;
        return 0;
    }
    case WM_MBUTTONUP:
    {
        s_mouse.buttons[BUTTON_MIDDLE] = false;
        return 0;
    }
    case WM_RBUTTONDOWN:
    {
        s_mouse.buttons[BUTTON_RIGHT] = true;
        return 0;
    }
    case WM_RBUTTONUP:
    {
        s_mouse.buttons[BUTTON_RIGHT] = false;
        return 0;
    }
    // Hide Window if Space is pressed
    case WM_KEYDOWN:
    {
        if (wParam == VK_SPACE)
        {
            ShowWindow(hWnd, SW_HIDE);
        }
        break;
    }

    case WM_DESTROY:
    case WM_QUIT:
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    return CallWindowProc(s_originalWndProc, hWnd, msg, wParam, lParam);
}

#endif // JPLATFORM_WINDOWS