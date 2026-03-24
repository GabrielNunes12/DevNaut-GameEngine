#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>

namespace Nova {

    class Shader {
    public:
        Shader() = default;
        ~Shader() = default;

        // Compile from source strings
        bool CompileFromSource(
            ID3D11Device* device,
            const std::string& vertexSrc,
            const std::string& pixelSrc
        );

        void Bind(ID3D11DeviceContext* ctx) const;

        ID3D11InputLayout* GetInputLayout() const { return m_InputLayout.Get(); }

    private:
        template<typename T>
        using ComPtr = Microsoft::WRL::ComPtr<T>;

        ComPtr<ID3D11VertexShader> m_VertexShader;
        ComPtr<ID3D11PixelShader>  m_PixelShader;
        ComPtr<ID3D11InputLayout>  m_InputLayout;
    };

} // namespace Nova
