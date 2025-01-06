#include "renderer/d3d11/gui/widget_d3d11.h"

#if JPLATFORM_WINDOWS

#include "renderer/d3d11/renderer_d3d11.h"
#include "logger.h"
#include "jmacros.h"
#include "renderer/vertex.h"

joj::D3D11Widget::D3D11Widget()
{
}

joj::D3D11Widget::D3D11Widget(const u16 x, const u16 y, const u16 width, const u16 height)
    : IWidget(x, y, width, height)
{
}

joj::D3D11Widget::D3D11Widget(const u16 x, const u16 y, const u16 width, const u16 height, const Color color)
    : IWidget(x, y, width, height, color)
{
}

joj::D3D11Widget::~D3D11Widget()
{
}

joj::ErrorCode joj::D3D11Widget::create(GraphicsDevice& device)
{
    const Vertex::PosColor quad_vertices[4] =
    {
        // Position
        {
            JFloat3{ m_normalized_x, m_normalized_y, 0.0f }, // Top Left
            JFloat4{ m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a }
        },
        {
            JFloat3{ m_normalized_x + m_normalized_width, m_normalized_y, 0.0f }, // Top Right
            JFloat4{ m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a }
        },
        {
            JFloat3{ m_normalized_x + m_normalized_width, m_normalized_y - m_normalized_height, 0.0f }, // Bottom Right
            JFloat4{ m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a }
        },
        {
            JFloat3{ m_normalized_x, m_normalized_y - m_normalized_height, 0.0f }, // Bottom Left
            JFloat4{ m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a }
        },
    };

    m_vb.setup(BufferUsage::Immutable, CPUAccessType::None,
        sizeof(Vertex::PosColor) * 4, quad_vertices);

    // Create Vertex Buffer
    if JOJ_FAILED(m_vb.create(device))
    {
        JERROR(ErrorCode::ERR_GUI_CANVAS_D3D11_CREATION,
            "Failed to create D3D11 Vertex Buffer for D3D11Button.");
        return ErrorCode::ERR_GUI_CANVAS_D3D11_CREATION;
    }

    const u32 quad_indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };

    m_ib.setup(sizeof(u32) * 6, quad_indices);

    // Create Index Buffer
    if JOJ_FAILED(m_ib.create(device))
    {
        JERROR(ErrorCode::ERR_GUI_CANVAS_D3D11_CREATION,
            "Failed to create D3D11 index buffer.");
        return ErrorCode::ERR_GUI_CANVAS_D3D11_CREATION;
    }

    // Create Constant Buffer
    m_cb.setup(joj::calculate_cb_byte_size(sizeof(CBHovered)), nullptr);
    JOJ_LOG_IF_FAIL(m_cb.create(device));

    // Compile and Create Shader
    m_shader.compile_vertex_shader_from_file("shaders/Canvas.hlsl", "VS", ShaderModel::Default);
    m_shader.compile_pixel_shader_from_file("shaders/Canvas.hlsl", "PS", ShaderModel::Default);
    JOJ_LOG_IF_FAIL(m_shader.create_vertex_shader(device));
    JOJ_LOG_IF_FAIL(m_shader.create_pixel_shader(device));

    // Create input layout
    InputDesc input_desc[] =
    {
        { "POSITION", 0, DataFormat::R32G32B32_FLOAT,    0,  0, InputClassification::PerVertexData, 0 },
        { "COLOR",    0, DataFormat::R32G32B32A32_FLOAT, 0, 12, InputClassification::PerVertexData, 0 },
    };

    for (auto& desc : input_desc)
        m_input_layout.add(desc);

    if JOJ_FAILED(m_input_layout.create(device, m_shader.get_vertex_shader()))
    {
        JERROR(ErrorCode::ERR_GUI_CANVAS_D3D11_CREATION,
            "Failed to create D3D11 Input Layout for D3D11Button.");
        return ErrorCode::ERR_GUI_CANVAS_D3D11_CREATION;
    }

    return ErrorCode::OK;
}

void joj::D3D11Widget::draw(CommandList& cmd_list)
{
    u32 stride = sizeof(Vertex::PosColor);
    u32 offset = 0;
    m_vb.bind(cmd_list, 0, 1, &stride, &offset);
    m_ib.bind(cmd_list, DataFormat::R32_UINT, offset);
    m_input_layout.bind(cmd_list);

    CBHovered cb_data;
    cb_data.hovered = 0;
    if (m_update)
    {
        cb_data.hovered_color = JFloat4{
            m_hovered_color.r, m_hovered_color.g,
            m_hovered_color.b, m_hovered_color.a
        };
        cb_data.hovered = 1;
        m_update = false;
    }
    m_cb.update(cmd_list, cb_data);

    m_cb.bind_to_vertex_shader(cmd_list, 0, 1);

    m_shader.bind_vertex_shader(cmd_list);
    m_shader.bind_pixel_shader(cmd_list);

    cmd_list.device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd_list.device_context->DrawIndexed(6, 0, 0);
}

b8 joj::D3D11Widget::is_hovered(const i32 x, const i32 y)
{
    return x >= m_x && x <= m_x + m_width && y >= m_y && y <= m_y + m_height;
}

void joj::D3D11Widget::update()
{
    m_update = true;
}

void joj::D3D11Widget::set_background_color(const Color color)
{
    m_background_color = color;
}

void joj::D3D11Widget::set_hovered_color(const Color color)
{
    m_hovered_color = color;
}

#endif // JPLATFORM_WINDOWS