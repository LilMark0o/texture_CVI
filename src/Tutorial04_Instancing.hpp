
#pragma once

#include "SampleBase.hpp"
#include "BasicMath.hpp"

namespace Diligent
{

class Tutorial04_Instancing final : public SampleBase
{
public:
    virtual void        Initialize(const SampleInitInfo& InitInfo) override final;
    virtual void        Render() override final;
    virtual void        Update(double CurrTime, double ElapsedTime) override final;
    virtual const Char* GetSampleName() const override final { return "Tutorial04: Instancing"; }

private:
    // Funciones para configurar el cubo instanciado
    void CreatePipelineState();
    void CreateInstanceBuffer();
    void PopulateInstanceBuffer();
    void UpdateUI();  // Ensure this function is declared

    // Recursos para los cubos
    RefCntAutoPtr<IPipelineState>         m_pPSO;
    RefCntAutoPtr<IBuffer>                m_CubeVertexBuffer;
    RefCntAutoPtr<IBuffer>                m_CubeIndexBuffer;
    RefCntAutoPtr<IBuffer>                m_InstanceBuffer;
    RefCntAutoPtr<IBuffer>                m_VSConstants;
    RefCntAutoPtr<ITextureView>           m_TextureSRV;
    RefCntAutoPtr<ITextureView>           m_TextureSRV_2;
    RefCntAutoPtr<ITextureView>           m_TextureSRV_3;

    RefCntAutoPtr<IShaderResourceBinding> m_SRB;

    // Recursos para el rectángulo (línea)
    RefCntAutoPtr<IPipelineState> m_RectPSO;
    RefCntAutoPtr<IBuffer>        m_RectVertexBuffer;
    RefCntAutoPtr<IBuffer>        m_RectVSConstants;

    float4x4             m_ViewProjMatrix;
    float4x4             m_RotationMatrix;
    static constexpr int NumInstances = 17; // Cambiado de 2 a 5
};

} // namespace Diligent
