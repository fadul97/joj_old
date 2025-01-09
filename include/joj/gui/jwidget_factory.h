#ifndef _JOJ_JWIDGET_FACTORY_H
#define _JOJ_JWIDGET_FACTORY_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "jbutton.h"
#include <string>
#include "jevent.h"
#include <memory>

namespace joj
{
    class JAPI JWidgetFactory
    {
    public:
        JWidgetFactory() = default;
        virtual ~JWidgetFactory() = default;

        virtual std::unique_ptr<JButton> create_button(i32 x, i32 y, i32 width, i32 height,
            const std::string& label, const JEvent::Callback& callback = nullptr) = 0;
    };
}

#endif // _JOJ_JWIDGET_FACTORY_H