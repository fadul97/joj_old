#include "hello_triangle.h"

#include "logger.h"
#include <sstream>

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "joj/math/jmath.h"
#include "joj/jmacros.h"
#include "joj/systems/light/light.h"

#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

using namespace DirectX;

struct LightBuffer
{
    joj::JFloat4 ambientColor;
    joj::JFloat4 diffuseColor;
    joj::JFloat3 lightDirection;
    f32 specularPower;
    joj::JFloat4 specularColor;
};

struct cbPerSkinned
{
    joj::JFloat4x4 gBoneTransforms[96];
};

struct cbPerObject
{
    joj::JFloat4x4 gWorld;
    joj::JFloat4x4 gWorldInvTranspose;
    joj::JFloat4x4 gWorldViewProj;
    joj::JFloat4x4 gWorldViewProjTex;
    joj::JFloat4x4 gTexTransform;
    joj::JFloat4x4 gShadowTransform;
    joj::Material gMaterial;
    u32 gUseTexure;
    u32 gAlphaClip;
    u32 gFogEnabled;
    u32 gReflectionEnabled;
};

struct cbPerFrame
{
    joj::DirectionalLight gDirLights[3];
    joj::JFloat3 gEyePosW;

    f32  gFogStart;
    f32  gFogRange;
    joj::JFloat4 gFogColor;
};

HelloTriangle::HelloTriangle()
{
    window = joj::Win32Window{ "jojWindow", 800, 600, joj::WindowMode::Windowed };
    input = joj::Win32Input();
    timer = joj::Win32Timer();
    renderer = joj::D3D11Renderer();

    using namespace DirectX;
    mDirLights[0].ambient = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
    mDirLights[0].diffuse = XMFLOAT4(0.8f, 0.7f, 0.7f, 1.0f);
    mDirLights[0].specular = XMFLOAT4(0.6f, 0.6f, 0.7f, 1.0f);
    mDirLights[0].direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

    mDirLights[1].ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    mDirLights[1].diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
    mDirLights[1].specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    mDirLights[1].direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

    mDirLights[2].ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    mDirLights[2].diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    mDirLights[2].specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    mDirLights[2].direction = XMFLOAT3(0.0f, 0.0, -1.0f);
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
    m_cam.set_pos(0.0f, 5.0f, -15.0f);
    m_cam.update_view_matrix();
    m_cam.set_lens(0.25f * J_PI, 800.0f / 600.0f, 0.1f, 1000.0f);
    m_cam.update_view_matrix();
    m_cam.look_at(m_cam.get_pos(), joj::JFloat3(0.0f, 0.0f, 0.0f), m_cam.get_up());
    m_cam.update_view_matrix();

    timer.start();

    m_static_shader.compile_vertex_shader_from_file(
        "../../../../samples/shaders/M3DTest.hlsl",
        "VS", joj::ShaderModel::Default);
    JOJ_LOG_IF_FAIL(m_static_shader.create_vertex_shader(renderer.get_device()));

    m_static_shader.compile_pixel_shader_from_file(
        "../../../../samples/shaders/M3DTest.hlsl",
        "PS", joj::ShaderModel::Default);
    JOJ_LOG_IF_FAIL(m_static_shader.create_pixel_shader(renderer.get_device()));

    m_static_shader.bind_vertex_shader(renderer.get_cmd_list());
    m_static_shader.bind_pixel_shader(renderer.get_cmd_list());

    joj::InputDesc layout[] = {
        { "POSITION", 0, joj::DataFormat::R32G32B32_FLOAT, 0,  0, joj::InputClassification::PerVertexData, 0 },
        { "NORMAL",   0, joj::DataFormat::R32G32B32_FLOAT, 0, 12, joj::InputClassification::PerVertexData, 0 },
        { "TEXCOORD", 0, joj::DataFormat::R32G32_FLOAT,    0, 24, joj::InputClassification::PerVertexData, 0 },
        { "TANGENT",  0, joj::DataFormat::R32G32B32_FLOAT, 0, 32, joj::InputClassification::PerVertexData, 0 },
    };

    for (auto& l : layout)
    {
        m_static_layout.add(l);
    }

    JOJ_LOG_IF_FAIL(m_static_layout.create(renderer.get_device(), m_static_shader.get_vertex_shader()));
    m_static_layout.bind(renderer.get_cmd_list());

    m_rock_model = joj::D3D11BasicModel();
    JOJ_LOG_IF_FAIL(m_rock_model.load_m3d(renderer.get_device(),
        renderer.get_cmd_list(),
        m_tex_mgr,
        "../../../../samples/models/rock.m3d",
        L"../../../../samples/textures/"));

    joj::BasicModelInstance rockInstance1;
    joj::BasicModelInstance rockInstance2;
    joj::BasicModelInstance rockInstance3;

    rockInstance1.model = &m_rock_model;
    rockInstance2.model = &m_rock_model;
    rockInstance3.model = &m_rock_model;

    XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
    XMMATRIX modelRot = XMMatrixRotationY(0.0f);
    XMMATRIX modelOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

    modelScale = XMMatrixScaling(0.8f, 0.8f, 0.8f);
    modelOffset = XMMatrixTranslation(-1.0f, 1.4f, -7.0f);
    XMStoreFloat4x4(&rockInstance1.world, modelScale * modelRot * modelOffset);

    modelScale = XMMatrixScaling(0.8f, 0.8f, 0.8f);
    modelOffset = XMMatrixTranslation(5.0f, 1.2f, -2.0f);
    XMStoreFloat4x4(&rockInstance2.world, modelScale * modelRot * modelOffset);

    modelScale = XMMatrixScaling(0.8f, 0.8f, 0.8f);
    modelOffset = XMMatrixTranslation(-4.0f, 1.3f, 3.0f);
    XMStoreFloat4x4(&rockInstance3.world, modelScale * modelRot * modelOffset);

    mModelInstances.push_back(rockInstance1);
    mModelInstances.push_back(rockInstance2);
    mModelInstances.push_back(rockInstance3);

    cbObject.setup(joj::calculate_cb_byte_size(sizeof(cbPerObject)), nullptr);
    JOJ_LOG_IF_FAIL(cbObject.create(renderer.get_device()));
    cbObject.bind_to_vertex_shader(renderer.get_cmd_list(), 0, 1);
    cbObject.bind_to_pixel_shader(renderer.get_cmd_list(), 0, 1);

    cbFrame.setup(joj::calculate_cb_byte_size(sizeof(cbPerFrame)), nullptr);
    JOJ_LOG_IF_FAIL(cbFrame.create(renderer.get_device()));
    cbFrame.bind_to_vertex_shader(renderer.get_cmd_list(), 1, 1);
    cbFrame.bind_to_pixel_shader(renderer.get_cmd_list(), 1, 1);

    // Describe a texture sampler.
    joj::SamplerDesc samplerDesc;
    samplerDesc.filter = joj::SamplerFilter::MIN_MAG_MIP_LINEAR;
    samplerDesc.addressU = joj::TextureAddressMode::Wrap;
    samplerDesc.addressV = joj::TextureAddressMode::Wrap;
    samplerDesc.addressW = joj::TextureAddressMode::Wrap;
    samplerDesc.mip_lod_bias = 0.0f;
    samplerDesc.max_anisotropy = 1;
    samplerDesc.func = joj::ComparisonFunc::Always;
    samplerDesc.border_color[0] = 0.0f;
    samplerDesc.border_color[0] = 0.0f;
    samplerDesc.border_color[0] = 0.0f;
    samplerDesc.border_color[0] = 0.0f;
    samplerDesc.min_LOD = joj::LODValue::Zero;
    samplerDesc.max_LOD = joj::LODValue::Float32_MAX;

    // Create Sampler State
    JOJ_LOG_IF_FAIL(m_sampler_state.create(renderer.get_device(), samplerDesc));
    m_sampler_state.bind(renderer.get_cmd_list(), joj::SamplerType::Anisotropic, 0, 1);
}

void HelloTriangle::update(const f32 dt)
{
    if (input.is_key_pressed('1'))
    {
        if (m_raster_state == joj::RasterizerState::Solid)
            m_raster_state = joj::RasterizerState::Wireframe;
        else
            m_raster_state = joj::RasterizerState::Solid;
    }

    if (input.is_key_down(joj::KEY_SPACE))
        JDEBUG("Space down.");

    if (input.is_key_pressed(joj::KEY_ESCAPE))
        loop = false;
}

void HelloTriangle::draw()
{
    renderer.clear();

    renderer.set_primitive_topology(joj::PrimitiveTopology::TRIANGLE_LIST);

    renderer.set_rasterizer_state(m_raster_state);
    
    draw_objects();

    renderer.swap_buffers();
}

void HelloTriangle::draw_one_object(u32 model_index)
{
    m_static_shader.bind_vertex_shader(renderer.get_cmd_list());
    m_static_shader.bind_pixel_shader(renderer.get_cmd_list());

    using namespace DirectX;

    XMMATRIX view = XMLoadFloat4x4(&m_cam.get_view());
    XMMATRIX proj = XMLoadFloat4x4(&m_cam.get_proj());
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);

    float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    cbPerFrame cbPF;
    cbPF.gDirLights[0] = mDirLights[0];
    cbPF.gDirLights[1] = mDirLights[1];
    cbPF.gDirLights[2] = mDirLights[2];
    cbPF.gEyePosW = m_cam.get_pos();
    cbFrame.update(renderer.get_cmd_list(), cbPF);

    m_static_layout.bind(renderer.get_cmd_list());
    renderer.get_cmd_list().device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    XMMATRIX world;
    XMMATRIX worldInvTranspose;
    XMMATRIX worldViewProj;

    // Transform NDC space [-1,+1]^2 to texture space [0,1]^2
    XMMATRIX toTexSpace(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

    UINT stride = sizeof(joj::Vertex::PosNormalTexTan);
    UINT offset = 0;

    world = XMLoadFloat4x4(&mModelInstances[model_index].world);
    worldInvTranspose = joj::inverse_transpose(world);
    worldViewProj = world * view * proj;
    auto worldViewProjTex = worldViewProj * toTexSpace;
    auto cbShadowTransform = world * shadowTransform;
    auto texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);

    cbPerObject cbPO;
    XMStoreFloat4x4(&cbPO.gWorld, XMMatrixTranspose(world));
    XMStoreFloat4x4(&cbPO.gWorldInvTranspose, XMMatrixTranspose(worldInvTranspose));
    XMStoreFloat4x4(&cbPO.gWorldViewProj, XMMatrixTranspose(worldViewProj));
    XMStoreFloat4x4(&cbPO.gWorldViewProjTex, XMMatrixTranspose(worldViewProjTex));
    XMStoreFloat4x4(&cbPO.gShadowTransform, XMMatrixTranspose(cbShadowTransform));
    XMStoreFloat4x4(&cbPO.gTexTransform, XMMatrixTranspose(texTransform));
    cbPO.gUseTexure = 1;
    cbPO.gAlphaClip = 0;
    cbPO.gFogEnabled = 0;
    cbPO.gReflectionEnabled = 0;

    for (u32 subset = 0; subset < mModelInstances[model_index].model->get_submesh_count(); ++subset)
    {
        cbPO.gMaterial = mModelInstances[model_index].model->get_mat()[subset];
        cbObject.update(renderer.get_cmd_list(), cbPO);

        /*
        auto& diffuse_map_SRV = mModelInstances[model_index].model->get_diffuse_map_SRV()[subset];
        auto& normal_map_SRV = mModelInstances[model_index].model->get_normal_map_SRV()[subset];
        */

        renderer.get_cmd_list().device_context->PSSetShaderResources(0, 1,
            &mModelInstances[model_index].model->get_diffuse_map_SRV()[subset]->srv);

        renderer.get_cmd_list().device_context->PSSetShaderResources(1, 1,
            &mModelInstances[model_index].model->get_normal_map_SRV()[subset]->srv);

        mModelInstances[model_index].model->get_mesh()->draw(renderer.get_cmd_list(), subset);
    }
}

void HelloTriangle::draw_objects()
{
    m_static_shader.bind_vertex_shader(renderer.get_cmd_list());
    m_static_shader.bind_pixel_shader(renderer.get_cmd_list());

    cbObject.bind_to_vertex_shader(renderer.get_cmd_list(), 0, 1);
    cbObject.bind_to_pixel_shader(renderer.get_cmd_list(), 0, 1);
    cbFrame.bind_to_vertex_shader(renderer.get_cmd_list(), 1, 1);
    cbFrame.bind_to_pixel_shader(renderer.get_cmd_list(), 1, 1);

    XMMATRIX view = XMLoadFloat4x4(&m_cam.get_view());
    XMMATRIX proj = XMLoadFloat4x4(&m_cam.get_proj());
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);

    float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    cbPerFrame cbPF;
    cbPF.gDirLights[0] = mDirLights[0];
    cbPF.gDirLights[1] = mDirLights[1];
    cbPF.gDirLights[2] = mDirLights[2];
    cbPF.gEyePosW = m_cam.get_pos();
    cbFrame.update(renderer.get_cmd_list(), cbPF);

    m_static_layout.bind(renderer.get_cmd_list());
    renderer.get_cmd_list().device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    XMMATRIX world;
    XMMATRIX worldInvTranspose;
    XMMATRIX worldViewProj;

    // Transform NDC space [-1,+1]^2 to texture space [0,1]^2
    XMMATRIX toTexSpace(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

    u32 stride = sizeof(joj::Vertex::PosNormalTexTan);
    u32 offset = 0;

    for (u32 model_index = 0; model_index < mModelInstances.size(); ++model_index)
    {
        world = XMLoadFloat4x4(&mModelInstances[model_index].world);
        worldInvTranspose = joj::inverse_transpose(world);
        worldViewProj = world * view * proj;
        auto worldViewProjTex = worldViewProj * toTexSpace;
        auto cbShadowTransform = world * shadowTransform;
        auto texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);

        cbPerObject cbPO;
        XMStoreFloat4x4(&cbPO.gWorld, XMMatrixTranspose(world));
        XMStoreFloat4x4(&cbPO.gWorldInvTranspose, XMMatrixTranspose(worldInvTranspose));
        XMStoreFloat4x4(&cbPO.gWorldViewProj, XMMatrixTranspose(worldViewProj));
        XMStoreFloat4x4(&cbPO.gWorldViewProjTex, XMMatrixTranspose(worldViewProjTex));
        XMStoreFloat4x4(&cbPO.gShadowTransform, XMMatrixTranspose(cbShadowTransform));
        XMStoreFloat4x4(&cbPO.gTexTransform, XMMatrixTranspose(texTransform));
        cbPO.gUseTexure = 1;
        cbPO.gAlphaClip = 0;
        cbPO.gFogEnabled = 0;
        cbPO.gReflectionEnabled = 0;

        for (u32 subset = 0; subset < mModelInstances[model_index].model->get_submesh_count(); ++subset)
        {
            cbPO.gMaterial = mModelInstances[model_index].model->get_mat()[subset];
            cbObject.update(renderer.get_cmd_list(), cbPO);

            /*
            auto& diffuse_map_SRV = mModelInstances[model_index].model->get_diffuse_map_SRV()[subset];
            auto& normal_map_SRV = mModelInstances[model_index].model->get_normal_map_SRV()[subset];
            */

            renderer.get_cmd_list().device_context->PSSetShaderResources(0, 1,
                &mModelInstances[model_index].model->get_diffuse_map_SRV()[subset]->srv);

            renderer.get_cmd_list().device_context->PSSetShaderResources(1, 1,
                &mModelInstances[model_index].model->get_normal_map_SRV()[subset]->srv);

            mModelInstances[model_index].model->get_mesh()->draw(renderer.get_cmd_list(), subset);
        }
    }
}

void HelloTriangle::shutdown()
{
    timer.end_period();
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