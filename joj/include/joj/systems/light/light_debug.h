#ifndef _JOJ_LIGHT_DEBUG_H
#define _JOJ_LIGHT_DEBUG_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "light.h"

namespace joj
{
    JAPI b8 are_directional_lights_equals(DirectionalLight& l1, DirectionalLight& l2);
    JAPI b8 are_point_lights_equals(PointLight& l1, PointLight& l2);
    JAPI b8 are_spot_lights_equals(SpotLight& l1, SpotLight& l2);
}

#endif // _JOJ_LIGHT_DEBUG_H