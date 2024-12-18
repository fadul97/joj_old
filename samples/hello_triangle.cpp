#include "hello_triangle.h"

#include "logger.h"
#include <sstream>

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "joj/math/jmath.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

using namespace DirectX;

ID3D11InputLayout* gInputLayout = nullptr;
ID3D11SamplerState* m_sampleState = nullptr;

struct CameraBufferType
{
    joj::JFloat3 cameraPosition;
    f32 padding;
};

struct LightBuffer
{
    joj::JFloat4 ambientColor;
    joj::JFloat4 diffuseColor;
    joj::JFloat3 lightDirection;
    f32 specularPower;
    joj::JFloat4 specularColor;
};

HelloTriangle::HelloTriangle()
{
    window = joj::Win32Window{ "jojWindow", 800, 600, joj::WindowMode::Windowed };
    input = joj::Win32Input();
    timer = joj::Win32Timer();
    renderer = joj::D3D11Renderer();
}

HelloTriangle::~HelloTriangle()
{

}

void HelloTriangle::init()
{
    if (window.create() != joj::ErrorCode::OK)
        return;

    u32 width = 0;
    u32 height = 0;

    window.get_window_size(width, height);
    JDEBUG("Window size: %dx%d", width, height);

    window.get_client_size(width, height);
    JDEBUG("Client size: %dx%d", width, height);

    input.set_window(window.get_data());

    timer.begin_period();

    renderer_print();

    if (renderer.initialize(window.get_data()) != joj::ErrorCode::OK)
        return;

    m_cam.update_view_matrix();
    m_cam.set_pos(0.0f, 5.0f, -20.0f);
    m_cam.update_view_matrix();
    m_cam.set_lens(0.25f * J_PI, 800.0f / 600.0f, 0.1f, 1000.0f);
    m_cam.update_view_matrix();
    m_cam.look_at(m_cam.get_pos(), joj::JFloat3(0.0f, 0.0f, 0.0f), m_cam.get_up());
    m_cam.update_view_matrix();

    timer.start();

    m_spaceship = joj::D3D11Mesh("../../../../samples/models/MySpaceShip.obj", joj::MeshType::OBJ);
    m_spaceship.setup(renderer.get_device());
    m_spaceship.translate(-10, 0, 10);

    m_spaceship2 = joj::D3D11Mesh("../../../../samples/models/MySpaceShip.obj", joj::MeshType::OBJ);
    m_spaceship2.setup(renderer.get_device());
    m_spaceship2.translate(10, 0, 10);

    m_light_cb.setup(joj::calculate_cb_byte_size(sizeof(LightBuffer)), nullptr);
    m_light_cb.create(renderer.get_device());

    m_camera_cb.setup(joj::calculate_cb_byte_size(sizeof(CameraBufferType)), nullptr);
    m_camera_cb.create(renderer.get_device());

    // Create a texture sampler state description.
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    if (renderer.get_device().device->CreateSamplerState(&samplerDesc, &m_sampleState) != S_OK)
    {
        JERROR(joj::ErrorCode::FAILED, "Failed to create D3D11 Sampler State.");
    }

    // Layout de entrada
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    renderer.get_device().device->CreateInputLayout(layout, 3,
        m_spaceship.get_shader().get_vertex_shader().vsblob->GetBufferPointer(),
        m_spaceship.get_shader().get_vertex_shader().vsblob->GetBufferSize(), &gInputLayout);
}

void HelloTriangle::update(const f32 dt)
{
    if (input.is_key_pressed('A'))
        JDEBUG("A pressed.");

    if (input.is_key_down(joj::KEY_SPACE))
        JDEBUG("Space down.");

    if (input.is_key_pressed(joj::KEY_ESCAPE))
        loop = false;


    static float rotation = 0.0f;
    // Update the rotation variable each frame.
    rotation -= 0.0174532925f * 0.1f;
    if (rotation < 0.0f)
    {
        rotation += 360.0f;
    }

    static float angle = 0.0f;
    angle += 0.01f;

    {
        m_spaceship.update(m_cam.get_view(), m_cam.get_proj(), dt);
        m_spaceship2.update(m_cam.get_view(), m_cam.get_proj(), dt);
    }

    {
        LightBuffer lightBuffer;
        lightBuffer.ambientColor = joj::JFloat4(0.15f, 0.15f, 0.15f, 1.0f);
        lightBuffer.diffuseColor = joj::JFloat4(1.0f, 1.0f, 1.0f, 1.0);
        lightBuffer.lightDirection = joj::JFloat3(1.0f, -1.0f, 1.0f);
        lightBuffer.specularColor = joj::JFloat4(0.0f, 0.0f, 1.0f, 1.0f);
        lightBuffer.specularPower = 32.0f;
        m_light_cb.update(renderer.get_cmd_list(), lightBuffer);
    }

    {
        CameraBufferType cameraBuffer;
        cameraBuffer.cameraPosition = m_cam.get_pos();
        m_camera_cb.update(renderer.get_cmd_list(), cameraBuffer);
    }
}

void HelloTriangle::draw()
{
    renderer.clear();

    renderer.get_cmd_list().device_context->IASetInputLayout(gInputLayout);
    renderer.get_cmd_list().device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    m_light_cb.bind_to_pixel_shader(renderer.get_cmd_list(), 0, 1);
    m_camera_cb.bind_to_vertex_shader(renderer.get_cmd_list(), 1, 1);

    renderer.get_cmd_list().device_context->PSSetSamplers(0, 1, &m_sampleState);

    m_spaceship.draw(renderer.get_device(), renderer.get_cmd_list());
    m_spaceship2.draw(renderer.get_device(), renderer.get_cmd_list());

    renderer.swap_buffers();
}

void HelloTriangle::shutdown()
{
    timer.end_period();

    m_sampleState->Release();
    gInputLayout->Release();
}

f32 HelloTriangle::get_frametime()
{
#ifdef JOJ_DEBUG_MODE
    static f32 total_time = 0.0f;	// Total time elapsed
    static u32  frame_count = 0;	// Elapsed frame counter
#endif // JOJ_DEBUG_MODE

    // Current frame time
    frametime = timer.reset();

#ifdef JOJ_DEBUG_MODE
    // Accumulated frametime
    total_time += frametime;

    // Increment frame counter
    frame_count++;

    // Updates FPS indicator in the window every 1000ms (1 second)
    if (total_time >= 1.0f)
    {
        std::stringstream text;		// Text flow for messages
        text << std::fixed;			// Always show the fractional part
        text.precision(3);			// three numbers after comma

        text << "Joj Engine v0.0.1" << "    "
            << "FPS: " << frame_count << "    "
            << "Frametime: " << frametime * 1000 << " (ms)";

        window.set_title(text.str().c_str());

        frame_count = 0;
        total_time -= 1.0f;
    }
#endif // JOJ_DEBUG_MODE

    return frametime;
}