#ifndef _JOJ_JWIDGET_FACTORY_H
#define _JOJ_JWIDGET_FACTORY_H

#include "joj/core/defines.h"

#include "jbutton.h"
#include <string>
#include "jgui_event.h"

namespace joj
{
    class JOJ_API JWidgetFactory
    {
    public:
        JWidgetFactory() = default;
        virtual ~JWidgetFactory() = default;

        virtual JButton* create_button(i32 x, i32 y, i32 width, i32 height,
            const std::string& label, const JGUIEvent::Callback& callback = nullptr) = 0;
    };
}

#endif // _JOJ_JWIDGET_FACTORY_H