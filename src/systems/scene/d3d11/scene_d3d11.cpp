#include "systems/scene/d3d11/scene_d3d11.h"

#if JPLATFORM_WINDOWS

#include "jmacros.h"
#include "renderer/d3d11/sprite_d3d11.h"
#include "renderer/vertex.h"
#include "renderer/d3d11/renderer_d3d11.h"

joj::D3D11Scene::D3D11Scene()
{
    m_sprite_vertex_buffer2D = D3D11VertexBuffer();
    m_sprite_index_buffer2D = D3D11IndexBuffer();
    m_sprite_constant_buffer = D3D11ConstantBuffer();
    m_sprite_shader = D3D11Shader();
    m_sprite_layout = D3D11InputLayout();
}

joj::D3D11Scene::~D3D11Scene()
{
}

struct ConstantBuffer2D
{
    joj::JFloat4x4 world;
    joj::JFloat4 color;
    joj::JFloat4 uv_rect;
};

void joj::D3D11Scene::init(const GraphicsDevice& device, Camera& camera)
{
    m_camera = &camera;

    m_sprite_shader.compile_vertex_shader_from_file(
        "shaders/Sprite.hlsl",
        "VS", joj::ShaderModel::Default);
    JOJ_LOG_IF_FAIL(m_sprite_shader.create_vertex_shader(device));

    m_sprite_shader.compile_pixel_shader_from_file(
        "shaders/Sprite.hlsl",
        "PS", joj::ShaderModel::Default);
    JOJ_LOG_IF_FAIL(m_sprite_shader.create_pixel_shader(device));

    joj::InputDesc layout[] = {
        { "POSITION", 0, joj::DataFormat::R32G32B32_FLOAT,    0,  0, joj::InputClassification::PerVertexData, 0 },
        { "COLOR",    0, joj::DataFormat::R32G32B32A32_FLOAT, 0, 12, joj::InputClassification::PerVertexData, 0 },
        { "TEXCOORD", 0, joj::DataFormat::R32G32_FLOAT,       0, 28, joj::InputClassification::PerVertexData, 0 },
    };

    for (auto& l : layout)
    {
        m_sprite_layout.add(l);
    }

    JOJ_LOG_IF_FAIL(m_sprite_layout.create(device, m_sprite_shader.get_vertex_shader()));

    joj::Vertex::PosColorUVRect quad_vertices[] =
    {
        { JFloat3{ -0.5f,  0.5f, 0.0f }, JFloat4{ 0.0f, 1.0f, 0.0f, 1.0f }, JFloat4{ 0.0f, 0.0f, 0.0f, 1.0f } }, // Top Left
        { JFloat3{  0.5f,  0.5f, 0.0f }, JFloat4{ 0.0f, 0.0f, 1.0f, 1.0f }, JFloat4{ 1.0f, 0.0f, 0.0f, 1.0f } }, // Top Right
        { JFloat3{  0.5f, -0.5f, 0.0f }, JFloat4{ 1.0f, 1.0f, 1.0f, 1.0f }, JFloat4{ 1.0f, 1.0f, 0.0f, 1.0f } }, // Bottom Right
        { JFloat3{ -0.5f, -0.5f, 0.0f }, JFloat4{ 1.0f, 0.0f, 0.0f, 1.0f }, JFloat4{ 0.0f, 1.0f, 0.0f, 1.0f } }, // Bottom Left
    };

    m_sprite_vertex_buffer2D.setup(BufferUsage::Immutable, CPUAccessType::None,
        sizeof(Vertex::PosColorUVRect) * 4, quad_vertices);
    JOJ_LOG_IF_FAIL(m_sprite_vertex_buffer2D.create(device));

    // �ndices para formar dois tri�ngulos
    u32 quad_indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };

    m_sprite_index_buffer2D.setup(sizeof(u32) * 6, quad_indices);
    JOJ_LOG_IF_FAIL(m_sprite_index_buffer2D.create(device));

    m_sprite_constant_buffer.setup(joj::calculate_cb_byte_size(sizeof(ConstantBuffer2D)), nullptr);
    JOJ_LOG_IF_FAIL(m_sprite_constant_buffer.create(device));
}

void joj::D3D11Scene::update(const f32 dt)
{
}

void joj::D3D11Scene::draw(IRenderer& renderer)
{
    m_sprite_shader.bind_vertex_shader(renderer.get_cmd_list());
    m_sprite_shader.bind_pixel_shader(renderer.get_cmd_list());
    m_sprite_layout.bind(renderer.get_cmd_list());

    u32 stride = sizeof(Vertex::PosColorUVRect);
    u32 offset = 0;

    m_sprite_vertex_buffer2D.bind(renderer.get_cmd_list(), 0, 1, &stride, &offset);
    m_sprite_index_buffer2D.bind(renderer.get_cmd_list(), joj::DataFormat::R32_UINT, 0);

    m_sprite_constant_buffer.bind_to_pixel_shader(renderer.get_cmd_list(), 0, 1);
    m_sprite_constant_buffer.bind_to_vertex_shader(renderer.get_cmd_list(), 0, 1);

    ConstantBuffer2D cb_data;
    for (auto& sprite : m_sprites)
    {
        SpriteData& sprite_data = sprite->get_sprite_data();

        JMatrix4x4 scale = DirectX::XMMatrixScaling(sprite_data.size.x, -sprite_data.size.y, 1.0f);
        JMatrix4x4 rotation = DirectX::XMMatrixRotationZ(sprite_data.rotation);
        JMatrix4x4 translation = DirectX::XMMatrixTranslation(sprite_data.position.x, sprite_data.position.y, 0.0f);
        JMatrix4x4 world = scale * rotation * translation;

        JMatrix4x4 view = DirectX::XMLoadFloat4x4(&m_camera->get_view());
        JMatrix4x4 proj = DirectX::XMLoadFloat4x4(&m_camera->get_proj());

        JMatrix4x4 wvp = world * view * proj;

        XMStoreFloat4x4(&cb_data.world, XMMatrixTranspose(wvp));
        cb_data.color = sprite_data.color;
        cb_data.uv_rect = sprite_data.uv_rect;
        m_sprite_constant_buffer.update(renderer.get_cmd_list(), cb_data);

        renderer.get_cmd_list().device_context->PSSetShaderResources(0, 1, &sprite_data.texture.srv);

        sprite->draw();
        renderer.draw_sprite(sprite_data);
    }
}

void joj::D3D11Scene::shutdown()
{
}

#endif // JPLATFORM_WINDOWS