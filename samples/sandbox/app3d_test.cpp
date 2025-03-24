﻿#include "app3d_test.h"

#include "joj/core/logger.h"
#include "joj/core/jmacros.h"
#include "joj/renderer/vertex.h"
#include "joj/engine/engine.h"
#include <math.h>
#include "joj/math/jvector3.h"
#include "joj/joj.h"

// Read file
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib> // rand()
#include <unordered_map>
#include "joj/renderer/shader_library.h"

inline std::vector<u8> load_binary_data(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open binary buffer file.");

    file.seekg(0, std::ios::end); // Go to the end of the file
    size_t size = file.tellg(); // Get the file size
    std::cout << "File size: " << size << std::endl;
    file.seekg(0, std::ios::beg); // Go back to the beginning of the file

    std::vector<u8> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);

    return data;
}

inline std::vector<joj::Vector3> load_positions_from_buffer(const std::vector<u8>& buffer, size_t byteOffset, size_t count)
{
    const joj::Vector3* data = reinterpret_cast<const joj::Vector3*>(buffer.data() + byteOffset);
    return std::vector<joj::Vector3>(data, data + count);
}

inline std::vector<uint16_t> load_indices_from_buffer(const std::vector<u8>& buffer, size_t byteOffset, size_t count)
{
    const uint16_t* data = reinterpret_cast<const uint16_t*>(buffer.data() + byteOffset);
    return std::vector<uint16_t>(data, data + count);
}

App3DTest::App3DTest()
{
    m_last_mouse_pos = { 0, 0 };
    joj_print();
}

App3DTest::~App3DTest()
{
}

void App3DTest::setup_camera()
{
    m_camera.update_view_matrix();
    m_camera.set_pos(0.0f, 5.0f, -15.0f);
    m_camera.update_view_matrix();
    m_camera.set_lens(0.25f * J_PI, 800.0f / 600.0f, 0.1f, 1000.0f);
    m_camera.update_view_matrix();
    m_camera.look_at(m_camera.get_pos(), joj::JFloat3(0.0f, 0.0f, 0.0f), m_camera.get_up());
    m_camera.update_view_matrix();
}

void App3DTest::build_buffers()
{
    const char* filename = "models/Cube.bin";
    auto data = load_binary_data(filename);

    constexpr size_t vertices_byteOffset = 0;
    constexpr size_t vertices_count = 24;
    auto positions = load_positions_from_buffer(data, vertices_byteOffset, vertices_count);

    constexpr size_t indices_byteOffset = 768;
    constexpr size_t indices_count = 36;
    auto indices = load_indices_from_buffer(data, indices_byteOffset, indices_count);

    // Print positions
    for (const auto& pos : positions)
        JOJ_INFO("Position: %.3f, %.3f, %.3f", pos.x, pos.y, pos.z);

    // Print indices
    for (const auto& index : indices)
        JOJ_INFO("Index: %d", index);

    // Array of Vector4 colors
    joj::Vector4 colors[4] =
    {
        { 1.0f, 0.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 0.0f, 1.0f }
    };

    // Loop positions to create vertices
    std::vector<joj::Vertex::PosColor> vertices;
    i32 i = 0;
    for (const auto& pos : positions)
    {
        vertices.push_back({ pos, colors[i] });
        i = (i + 1) % 4;
    }
    
    // Create vertex buffer
    const u32 vertices_size = static_cast<u32>(vertices.size());
    m_vertex_cout = vertices_size;
    JOJ_DEBUG("Vertices count: %d", m_vertex_cout);
    m_vb = joj::D3D11VertexBuffer(m_renderer->get_device(), m_renderer->get_cmd_list());
    if (m_vb.create(joj::BufferUsage::Default, joj::CPUAccessType::None,
        sizeof(joj::Vertex::RectUIType) * vertices_size, vertices.data()) != joj::ErrorCode::OK)
        return;

    // Create index buffer
    const u32 indices_size = static_cast<u32>(indices.size());
    m_index_count = indices_size;
    JOJ_DEBUG("Indices count: %d", m_index_count);
    m_ib = joj::D3D11IndexBuffer(m_renderer->get_device(), m_renderer->get_cmd_list());
    if (m_ib.create(joj::BufferUsage::Default, joj::CPUAccessType::None, sizeof(u16) * indices_size, indices.data()) != joj::ErrorCode::OK)
        return;

    // Create shader
    m_shader = joj::D3D11Shader(m_renderer->get_device(), m_renderer->get_cmd_list());
    if (m_shader.compile_vertex_shader(joj::ShaderLibrary::VertexShaderSimpleCamera, "VS", joj::ShaderModel::Default) != joj::ErrorCode::OK)
        return;

    if (m_shader.compile_pixel_shader(joj::ShaderLibrary::PixelShaderSimple, "PS", joj::ShaderModel::Default) != joj::ErrorCode::OK)
        return;

    std::vector<joj::InputDesc> layout =
    {
        { "POSITION", 0, joj::DataFormat::R32G32B32_FLOAT,    0,  0, joj::InputClassification::PerVertexData, 0 },
        { "COLOR",    0, joj::DataFormat::R32G32B32A32_FLOAT, 0, 12, joj::InputClassification::PerVertexData, 0 }
    };

    if (m_shader.create_input_layout(layout) != joj::ErrorCode::OK)
        return;

    m_cb = joj::D3D11ConstantBuffer(m_renderer->get_device(), m_renderer->get_cmd_list());
    if (m_cb.create(joj::BufferUsage::Dynamic, joj::CPUAccessType::Write, joj::calculate_cb_byte_size(sizeof(WVPBuffer)), nullptr) != joj::ErrorCode::OK)
        return;
}

void App3DTest::init()
{
    setup_camera();
    build_buffers();
    m_renderer->set_rasterizer_state(joj::RasterizerState::Solid);
}

void App3DTest::update(const f32 dt)
{
    if (m_input->is_key_pressed(joj::KEY_ESCAPE))
        joj::Engine::close();

    const f32 speed = 10.0f * dt;;

    process_mouse_input(dt);
}

static f32 angleY = 0.0f;
void App3DTest::draw()
{
    m_renderer->begin_frame();
    {
        m_cb.bind_to_vertex_shader(0, 1);
        joj::JMatrix4x4 v = DirectX::XMLoadFloat4x4(&m_camera.get_view());
        joj::JMatrix4x4 p = DirectX::XMLoadFloat4x4(&m_camera.get_proj());
        joj::JMatrix4x4 wvp = DirectX::XMMatrixTranspose(v * p);
        WVPBuffer wvp_buffer;
        DirectX::XMStoreFloat4x4(&wvp_buffer.wvp, wvp);
        m_cb.update(wvp_buffer);

        // Bind and Draw the rect
        constexpr u32 stride = sizeof(joj::Vertex::PosColor);
        constexpr u32 offset = 0;
        m_vb.bind(0, 1, &stride, &offset);
        m_ib.bind(joj::DataFormat::R16_UINT, offset);
        m_shader.bind();
        m_renderer->draw_indexed(m_index_count, 0, 0);
    }
    m_renderer->end_frame();
}

void App3DTest::shutdown()
{
}

void App3DTest::on_mouse_down(joj::Buttons button, i32 x, i32 y)
{
    m_last_mouse_pos.x = x;
    m_last_mouse_pos.y = y;

    SetCapture(m_window->get_data()->handle);
}

void App3DTest::on_mouse_up(joj::Buttons button, i32 x, i32 y)
{
    ReleaseCapture();
}

void App3DTest::on_mouse_move(WPARAM button_state, i32 x, i32 y)
{
    if ((button_state & MK_RBUTTON) != 0)
    {
        f32 dx = DirectX::XMConvertToRadians(0.25f * static_cast<f32>(x - m_last_mouse_pos.x));
        f32 dy = DirectX::XMConvertToRadians(0.25f * static_cast<f32>(y - m_last_mouse_pos.y));

        m_camera.pitch(dy);
        m_camera.rotateY(dx);
    }

    m_last_mouse_pos.x = x;
    m_last_mouse_pos.y = y;
}

void App3DTest::process_mouse_input(const f32 dt)
{
    const f32 speed = dt * 40.0f;

    if (m_input->is_key_down('W'))
        m_camera.walk(speed);

    if (m_input->is_key_down(joj::KEY_S))
        m_camera.walk(-speed);

    if (m_input->is_key_down(joj::KEY_A))
        m_camera.strafe(-speed);

    if (m_input->is_key_down(joj::KEY_D))
        m_camera.strafe(speed);

    m_camera.update_view_matrix();
}