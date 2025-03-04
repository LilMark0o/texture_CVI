
#include <random>
#include <cstring> // Include for strcmp
#include <string>
#include "Tutorial04_Instancing.hpp"
#include "MapHelper.hpp"
#include "GraphicsUtilities.h"
#include "TextureUtilities.h"
#include "ColorConversion.h"
#include "../../Common/src/TexturedCube.hpp"
#include "imgui.h"

namespace Diligent
{

// Función de fábrica para crear la muestra
SampleBase* CreateSample()
{
    return new Tutorial04_Instancing();
}

void Tutorial04_Instancing::CreatePipelineState()
{
    // Definición de la estructura de entrada con datos por vértice e instanciado
    LayoutElement LayoutElems[] =
        {
            LayoutElement{0, 0, 3, VT_FLOAT32, False},
            LayoutElement{1, 0, 2, VT_FLOAT32, False},
            LayoutElement{2, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
            LayoutElement{3, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
            LayoutElement{4, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE},
            LayoutElement{5, 1, 4, VT_FLOAT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE}};

    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(nullptr, &pShaderSourceFactory);

    // Configuración del PSO usando el helper de TexturedCube
    TexturedCube::CreatePSOInfo CubePsoCI;
    CubePsoCI.pDevice              = m_pDevice;
    CubePsoCI.RTVFormat            = m_pSwapChain->GetDesc().ColorBufferFormat;
    CubePsoCI.DSVFormat            = m_pSwapChain->GetDesc().DepthBufferFormat;
    CubePsoCI.pShaderSourceFactory = pShaderSourceFactory;

#ifdef DILIGENT_USE_OPENGL
    // Para OpenGL se usan nombres de archivos de shader adecuados (por ejemplo, GLSL)
    CubePsoCI.VSFilePath = "cube_inst_glsl.vert";
    CubePsoCI.PSFilePath = "cube_inst_glsl.frag";
#else
    CubePsoCI.VSFilePath = "cube_inst.vsh";
    CubePsoCI.PSFilePath = "cube_inst.psh";
#endif

    CubePsoCI.ExtraLayoutElements    = LayoutElems;
    CubePsoCI.NumExtraLayoutElements = _countof(LayoutElems);

    m_pPSO = TexturedCube::CreatePipelineState(CubePsoCI, m_ConvertPSOutputToGamma);

    // Creación del uniform buffer para las constantes del VS
    CreateUniformBuffer(m_pDevice, sizeof(float4x4) * 2, "VS constants CB", &m_VSConstants);
    m_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_VSConstants);
    m_pPSO->CreateShaderResourceBinding(&m_SRB, true);
}

void Tutorial04_Instancing::CreateInstanceBuffer()
{
    BufferDesc InstBuffDesc;
    InstBuffDesc.Name      = "Instance data buffer";
    InstBuffDesc.Usage     = USAGE_DEFAULT;
    InstBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
    InstBuffDesc.Size      = sizeof(float4x4) * NumInstances;
    m_pDevice->CreateBuffer(InstBuffDesc, nullptr, &m_InstanceBuffer);
    PopulateInstanceBuffer();
}

void Tutorial04_Instancing::Initialize(const SampleInitInfo& InitInfo)
{
    SampleBase::Initialize(InitInfo);
    CreatePipelineState();

    // Creación de los buffers e inicialización de recursos del cubo
    m_CubeVertexBuffer = TexturedCube::CreateVertexBuffer(m_pDevice, GEOMETRY_PRIMITIVE_VERTEX_FLAG_POS_TEX);
    m_CubeIndexBuffer  = TexturedCube::CreateIndexBuffer(m_pDevice);
    //m_TextureSRV       = TexturedCube::LoadTexture(m_pDevice, "DGLogo.png")->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
    TextureLoadInfo loadInfo;
    loadInfo.IsSRGB = true;
    loadInfo.Name   = "Cube Color Map";
    RefCntAutoPtr<ITexture> ColorMap;
    CreateTextureFromFile("texture.jpeg", loadInfo, m_pDevice, &ColorMap);
    m_TextureSRV = ColorMap->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);

    m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(m_TextureSRV);

    CreateInstanceBuffer();
}
static float PI_marco = 3.1415927f;

static float angle = 0.0f;   // Ángulo en radianes (puede incrementarse con el tiempo)
static float angle2 = 0*(PI_marco/3);   // Ángulo en radianes (puede incrementarse con el tiempo)
static float angle2_2 = 0*(PI_marco/3);   // Ángulo en radianes (puede incrementarse con el tiempo)
static float angle3 = (PI_marco);   // Ángulo en radianes (puede incrementarse con el tiempo)
static float angle4 = 0.4f+0*(PI_marco/3);   // Ángulo en radianes (puede incrementarse con el tiempo)

static float4x4 toFollow;
static int needed = 0;

void Tutorial04_Instancing::PopulateInstanceBuffer()
{
    std::vector<float4x4> InstanceData(NumInstances);
    angle += 0.01f;
    angle2 += 0.01f;
    angle2_2 += 0.01f/2;
    angle3 += 0.01f;
    angle4 += 0.01f;

    // Se definen las transformaciones de traslación para las 5 instancias
    InstanceData[0] = float4x4::Translation(0.0f, 9.0f, 0);
    InstanceData[1] = Matrix4x4<float>::Translation(4.75f, 5.0f, -2.f) * Matrix4x4<float>::RotationY(angle2);
    InstanceData[2] = Matrix4x4<float>::Translation(4.75f, 5.0f, -2.f) * Matrix4x4<float>::RotationY(angle3);
    InstanceData[3] = Matrix4x4<float>::Translation(0.0f, 5.0f, 0);
    InstanceData[4] = float4x4::Scale(4.5f, 0.01f, 0.01f) * float4x4::Translation(0.0f, 7.0f, 0.f) *  Matrix4x4<float>::RotationY(angle4);
    InstanceData[5] = float4x4::Scale(0.01f, 1.0f, 0.01f) * float4x4::Translation(-4.75f, 6.0f, 0)* Matrix4x4<float>::RotationY(angle4);
    InstanceData[6] = float4x4::Scale(0.01f, 1.0f, 0.01f) * float4x4::Translation(4.75f, 6.0f, 0)* Matrix4x4<float>::RotationY(angle4);
    InstanceData[7] = float4x4::Scale(0.01f, 2.0f, 0.01f) * float4x4::Translation(0.0f, 7.0f, 0);
    InstanceData[8] = float4x4::Scale(0.01f, 1.0f, 0.01f) * float4x4::Translation(4.75f, 4.0f, 0)* Matrix4x4<float>::RotationY(angle4);
    InstanceData[9] = Matrix4x4<float>::Translation(4.75f, 2.0f, -2.f) * Matrix4x4<float>::RotationY(angle2);
    InstanceData[10] = float4x4::Scale(0.01f, 1.0f, 0.01f) * float4x4::Translation(4.75f, 0.0f, 0)* Matrix4x4<float>::RotationY(angle4);
    InstanceData[11] =  float4x4::Scale(4.5f, 0.01f, 0.01f) * float4x4::Translation(4.75f, 0.0f, 0.f) *  Matrix4x4<float>::RotationY(angle4);
    InstanceData[12] = Matrix4x4<float>::Translation(4.75f, -4.0f, -2.f) * InstanceData[9];
    InstanceData[13] = Matrix4x4<float>::Translation(0.0f, -4.0f, 0.f) * InstanceData[9];
    InstanceData[14] = Matrix4x4<float>::Translation(-4.75f, -4.0f, +2.f) * InstanceData[9];
    InstanceData[15] = float4x4::Scale(0.01f, 1.0f, 0.01f) * Matrix4x4<float>::Translation(4.75f, -3.0f, -2.f) * InstanceData[9];
    InstanceData[16] = float4x4::Scale(0.01f, 1.0f, 0.01f) * Matrix4x4<float>::Translation(-4.75f, -3.0f, 2.f) * InstanceData[9];
    
    if (needed != 0){
        toFollow = InstanceData[14];
    }

    Uint32 DataSize = static_cast<Uint32>(sizeof(InstanceData[0]) * InstanceData.size());
    m_pImmediateContext->UpdateBuffer(m_InstanceBuffer, 0, DataSize, InstanceData.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void Tutorial04_Instancing::Render()
{
    auto*  pRTV       = m_pSwapChain->GetCurrentBackBufferRTV();
    auto*  pDSV       = m_pSwapChain->GetDepthBufferDSV();
    
    PopulateInstanceBuffer();
    
    float4 ClearColor = {0.350f, 0.350f, 0.350f, 1.0f};

    if (m_ConvertPSOutputToGamma)
        ClearColor = LinearToSRGB(ClearColor);

    m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor.Data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    {
        // Actualización del uniform buffer con la matriz de vista/proyección y la matriz de rotación
        MapHelper<float4x4> CBConstants(m_pImmediateContext, m_VSConstants, MAP_WRITE, MAP_FLAG_DISCARD);
        CBConstants[0] = m_ViewProjMatrix;
        CBConstants[1] = m_RotationMatrix;
    }

    const Uint64 offsets[] = {0, 0};
    IBuffer*     pBuffs[]  = {m_CubeVertexBuffer, m_InstanceBuffer};
    m_pImmediateContext->SetVertexBuffers(0, _countof(pBuffs), pBuffs, offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
    m_pImmediateContext->SetIndexBuffer(m_CubeIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    m_pImmediateContext->SetPipelineState(m_pPSO);
    m_pImmediateContext->CommitShaderResources(m_SRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    DrawIndexedAttribs DrawAttrs;
    DrawAttrs.IndexType    = VT_UINT32;
    DrawAttrs.NumIndices   = 36;
    DrawAttrs.NumInstances = NumInstances;
    DrawAttrs.Flags        = DRAW_FLAG_VERIFY_ALL;
    m_pImmediateContext->DrawIndexed(DrawAttrs);
}
static int selected_gui = 0;
float distanceX = 0.0f;
float distanceY = 0.0f;
float distanceZ = 30.0f;
void Tutorial04_Instancing::UpdateUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static const char* items[] = { "New", "Top", "Front", "Normal" };
        static int selectedItem = 0; // Index of the selected item
        static const char* selectedText = items[selectedItem]; // Store selected text

        if (ImGui::Combo("Grid Size", &selectedItem, items, IM_ARRAYSIZE(items)))
        {
            selected_gui = selectedItem;
        }

        // Example usage of selectedText
        ImGui::Text("Selected: %s", selectedText);
        ImGui::Text("WASD for moving the camara");
        ImGui::Text("Up arrow and down arrow for zoom and de-zoom");
    }
    ImGuiIO& io = ImGui::GetIO();
    if (io.KeysDown[49])  // Directly checking key state
      {
          needed = 0;
      }
    if (io.KeysDown[50])  // Directly checking key state
      {
          needed = 1;
      }

    if (io.KeysDown[87])  // Directly checking key state
      {
          distanceY = distanceY - 0.5f;


      }
    if (io.KeysDown[83])  // Directly checking key state
      {
          distanceY = distanceY + 0.5f;

      }
    
    if (io.KeysDown[257])  // Directly checking key state
      {
          distanceZ = distanceZ - 0.5f;


      }
    if (io.KeysDown[256])  // Directly checking key state
      {
          distanceZ = distanceZ + 0.5f;

      }
    if (io.KeysDown[65])  // Directly checking key state
      {
          distanceX = distanceX + 0.5f;


      }
    if (io.KeysDown[68])  // Directly checking key state
      {
          distanceX = distanceX - 0.5f;

      }
    ImGui::End();
}
void Tutorial04_Instancing::Update(double CurrTime, double ElapsedTime)
{
    // Set up a top-down view
    float4x4 View;
    if (needed != 0){
        if (distanceZ == 30.0f){
        distanceZ = 5;}
        View = float4x4::Translation(distanceX, distanceY, distanceZ) * toFollow;
    }else{
        if (selected_gui == 3)
        {
            View = float4x4::RotationX(-0.4f) * float4x4::Translation(distanceX, distanceY, distanceZ);
        }
        else if (selected_gui == 1)
        {
            View = float4x4::RotationX(-PI_F / 2) * float4x4::Translation(distanceX, distanceY, distanceZ);
        }
        else if (selected_gui == 2)
        {
            View = float4x4::RotationY(-PI_F / 2) * float4x4::Translation(distanceX, distanceY, distanceZ);
        }
        else {
            View = float4x4::RotationX(-PI_F / 6) * float4x4::RotationY(-PI_F / 4) * float4x4::Translation(distanceX, distanceY, distanceZ);
        }
    }
    

    SampleBase::Update(CurrTime, ElapsedTime);
    UpdateUI();

    
    auto SrfPreTransform = GetSurfacePretransformMatrix(float3{0, 0, 1});
    auto Proj = GetAdjustedProjectionMatrix(PI_F / 4.0f, 0.1f, 100.f);

    m_ViewProjMatrix = View * SrfPreTransform * Proj;
    m_RotationMatrix = float4x4::RotationY(static_cast<float>(CurrTime) * 1.0f);
}
} // namespace Diligent
