#ifndef _JOJ_SCENE_H
#define _JOJ_SCENE_H

#include "joj/core/defines.h"

#include "joj/renderer/renderer.h"
#include "joj/systems/camera/camera.h"
#include <vector>
#include "joj/renderer/sprite.h"
#include "joj/systems/physics/geometry.h"

namespace joj
{
    class JOJ_API Scene
    {
    public:
        Scene();
        virtual ~Scene();

        virtual void init(const GraphicsDevice& device, Camera& camera) = 0;
        virtual void update(const f32 dt) = 0;
        virtual void draw(IRenderer& renderer) = 0;
        virtual void shutdown() = 0;

        virtual void draw_collisions(IRenderer& renderer) = 0;

        void add_sprite(Sprite* sprite);

        void add_geometry(Geometry* geometry);
    
    protected:
        Camera* m_camera;
        std::vector<Sprite*> m_sprites;
        std::vector<Geometry*> m_geometries;
    };

    inline void Scene::add_sprite(Sprite* sprite)
    { m_sprites.push_back(sprite); }

    inline void Scene::add_geometry(Geometry* geometry)
    { m_geometries.push_back(geometry); }
}

#endif // _JOJ_SCENE_H