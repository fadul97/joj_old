#include "gui/jbutton.h"

#include "logger.h"

joj::JButton::JButton()
    : JWidget(), m_label("Button"), m_bounds({ 0, 0, 0, 0 }), m_handle({ nullptr })
{
}

joj::JButton::JButton(const u32 x, const u32 y, const u32 width, const u32 height,
    const char* title)
    : JWidget(x, y, width, height, title), m_label(title),
    m_bounds({ x, y, x + width, y + height }), m_handle({ nullptr })
{
}

joj::JButton::~JButton()
{
    if (m_handle.handle)
    {
        unregister_widget(m_handle);
        DestroyWindow(m_handle.handle);
    }
}

void joj::JButton::create(JWidgetCreationData& data)
{
    m_handle.handle = CreateWindowEx(
        0,
        "BUTTON",
        m_label.c_str(),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        m_bounds.left, m_bounds.top,
        m_bounds.right - m_bounds.left,
        m_bounds.bottom - m_bounds.top,
        data.parent_handle,
        nullptr,
        data.instance,
        nullptr
    );

    if (m_handle.handle)
        register_widget(m_handle);
}

void joj::JButton::draw(CommandList& cmd_list)
{
}

void joj::JButton::update(i32 xmouse, i32 ymouse, b8 clicked)
{
    /*
    typedef struct tagRECT
    {
        LONG    left;
        LONG    top;
        LONG    right;
        LONG    bottom;
    } RECT, *PRECT, NEAR *NPRECT, FAR *LPRECT;
    */

    /*
    RECT bounds = { m_bounds.left, m_bounds.top, m_bounds.right, m_bounds.bottom };

    m_is_hovered = is_hovered(xmouse, ymouse);
    // JDEBUG("Button hovered: %d, Mouse: %d,%d", m_is_hovered, xmouse, ymouse);

    if (m_is_hovered && clicked)
    {
        m_on_click.trigger();
    }
    */
}

b8 joj::JButton::is_hovered(const i32 x, const i32 y)
{
    return x >= m_x && x <= m_x + m_width && y >= m_y && y <= m_y + m_height;
}

void joj::JButton::on_click(const JEvent::Callback& callback)
{
    m_on_click.set_callback(callback);
}

LRESULT joj::JButton::handle_message(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED)
        {
            m_on_click.trigger();
        }
        break;
    }
    return DefWindowProc(m_handle.handle, msg, wParam, lParam);
}