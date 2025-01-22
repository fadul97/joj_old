#ifndef _JOJ_CAMERA_2D_H
#define _JOJ_CAMERA_2D_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "camera.h"

namespace joj
{
    class JAPI Camera2D : public Camera
    {
    public:
        Camera2D();
        Camera2D(const f32 left, const f32 right, const f32 bottom, const f32 top);
        ~Camera2D();

        void set_position(const JFloat3& position) override;
        void set_position(const f32 x, const f32 y, const f32 z) override;
        void set_rotation(const JFloat3& rotation) override;

        JFloat3 get_position() const override;

        void translate(const f32 dx, const f32 dy, const f32 dz) override;
        void move_to(const f32 x, const f32 y, const f32 z) override;
        void move(const JFloat3& offset) override;

    protected:
        void update() override;
    };
}

#endif // _JOJ_CAMERA_2D_H