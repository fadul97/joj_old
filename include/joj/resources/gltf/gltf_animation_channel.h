#ifndef _JOJ_GLTF_ANIMATION_CHANNEL_H
#define _JOJ_GLTF_ANIMATION_CHANNEL_H

#include "joj/core/defines.h"

#include "joj/resources/animation_channel_type.h"

namespace joj
{
    struct JOJ_API GLTFAnimationChannel
    {
        GLTFAnimationChannel();
        ~GLTFAnimationChannel();

        i32 sampler;
        i32 target_node;
        AnimationChannelType type;
    };
}

#endif // _JOJ_GLTF_ANIMATION_CHANNEL_H