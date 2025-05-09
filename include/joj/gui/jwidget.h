#ifndef _JOJ_JWIDGET_H
#define _JOJ_JWIDGET_H

#include "joj/core/defines.h"

#include <Windows.h>
#include "jgui_event.h"
#include <unordered_map>
#include "joj/core/logger.h"

#define IDC_BUTTON 201

namespace joj
{
    struct JOJ_API JWidgetCreationData
    {
        HWND parent_handle;
        HINSTANCE instance;
    };

    struct CommandList;

    struct WidgetHandle
    {
        HWND handle;
    };

    class JOJ_API JWidget
    {
    public:
        JWidget();
        JWidget(const u32 x, const u32 y, const u32 width, const u32 height,
            const char* title);
        virtual ~JWidget() = default;

        virtual void create(JWidgetCreationData& data, const JGUIEvent::Callback& callback) = 0;
        virtual void draw(CommandList& cmd_list) = 0;
        virtual void update(i32 xmouse, i32 ymouse, b8 clicked) = 0;

        virtual b8 is_hovered(const i32 x, const i32 y) = 0;

        virtual void on_click(const JGUIEvent::Callback& callback) = 0;
        virtual void trigger() = 0;

        void register_widget(WidgetHandle& handle);
        void unregister_widget(WidgetHandle& handle);

        using WidgetMap = std::unordered_map<HWND, JWidget*>;
        static WidgetMap& get_widget_map();

        virtual LRESULT handle_message(UINT msg, WPARAM wParam, LPARAM lParam) = 0;

    protected:
        u32 m_x;
        u32 m_y;
        u32 m_width;
        u32 m_height;

        JGUIEvent m_on_click;
        b8 m_is_hovered;

        static WidgetMap g_widget_map;
        static WNDPROC s_originalWndProc;
    };

    inline void JWidget::register_widget(WidgetHandle& handle)
    {
        g_widget_map[handle.handle] = this;
        JDEBUG("Registered widget with handle: %p", handle.handle);
    }

    inline void JWidget::unregister_widget(WidgetHandle& handle)
    {
        g_widget_map.erase(handle.handle);
    }

    inline std::unordered_map<HWND, JWidget*>& JWidget::get_widget_map()
    {
        return g_widget_map;
    }
}

#endif // _JOJ_JWIDGET_H