/*! \file tdxscene.h
    \brief TDXSceneクラスの実装

    Copyright ©  2015 @dc1394 All Rights Reserved.
    This software is released under the BSD-2 License.
*/

#include "DXUT.h"
#include "DXUTmisc.h"
#include "myrandom/myrand.h"
#include "resource.h"
#include "TDXScene.h"
#include <mutex>                                                // for std::mutex
#include <boost/assert.hpp>                                     // for BOOST_ASSERT
#include <boost/math/special_functions/spherical_harmonic.hpp>  // for boost::math::spherical_harmonic
#include <boost/range/algorithm.hpp>                            // for boost::fill
#include <gsl/gsl_sf_legendre.h>                                // for gsl_sf_legendre_sphPlm
#include <tbb/parallel_for.h>                                   // for tbb::parallel_for
#include <tbb/partitioner.h>                                    // for tbb::auto_partitioner
#include <tbb/task_scheduler_init.h>                            // for tbb::task_scheduler_init

namespace tdxscene {
    TDXScene::TDXScene(std::shared_ptr<getdata::GetData> const & pgd) :
        Pgd(nullptr, [this](std::shared_ptr<getdata::GetData> const & val) { return pgd_ = val; }),
        Redraw(nullptr, [this](bool redraw){ rewritewithlock(redraw_, redraw); }),
        Th([this]{ return th_; }, nullptr),
        Thread_end(nullptr, [this](bool thread_end){ rewritewithlock(thread_end_, thread_end); }),
        lightDirVariable(nullptr),
        meshColor(0.7f, 0.7f, 0.7f, 1.0f),
        meshColorVariable(nullptr),
        projectionVariable(nullptr),
        pgd_(pgd),
        technique(nullptr),
        textureRV(nullptr),
        vertices_(N),
        viewVariable(nullptr),
        worldVariable(nullptr)
    {
    }


    HRESULT TDXScene::Init(ID3D10Device* pd3dDevice)
    {
        // Read the D3DX effect file
        auto dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
        // Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
        // Setting this flag improves the shader debugging experience, but still allows 
        // the shaders to be optimized and to run exactly the way they will run in 
        // the release configuration of this program.
        dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

        ID3D10Effect * effecttmp;

        auto hr = D3DX10CreateEffectFromFile(
            L"SchracVisualize.fx",
            nullptr,
            nullptr,
            "fx_4_0",
            dwShaderFlags,
            0,
            pd3dDevice,
            nullptr,
            nullptr,
            &effecttmp,
            nullptr,
            nullptr);

        effect.reset(effecttmp);

        if (FAILED(hr))
        {
            ::MessageBox(nullptr,
                L"The FX file cannot be located.  Please run this executable from the directory that contains the FX file.",
                L"Error",
                MB_OK);
            if (!utility::v_return(hr)) {
                return S_FALSE;
            }
        }

        technique = effect->GetTechniqueByName("Render2");
        worldVariable = effect->GetVariableByName("World")->AsMatrix();
        viewVariable = effect->GetVariableByName("View")->AsMatrix();
        projectionVariable = effect->GetVariableByName("Projection")->AsMatrix();
        meshColorVariable = effect->GetVariableByName("vMeshColor")->AsVector();
        diffuseVariable = effect->GetVariableByName("txDiffuse")->AsShaderResource();

        // Define the input layout
        D3D10_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        };

        auto const numElements = sizeof(layout) / sizeof(layout[0]);

        // Create the input layout
        D3D10_PASS_DESC PassDesc;
        technique->GetPassByIndex(0)->GetDesc(&PassDesc);
        ID3D10InputLayout * pVertexLayout;

        if (!utility::v_return(
            pd3dDevice->CreateInputLayout(
            layout,
            numElements,
            PassDesc.pIAInputSignature,
            PassDesc.IAInputSignatureSize,
            &pVertexLayout))) {
            return S_FALSE;
        }

        vertexLayout.reset(pVertexLayout);

        // Set the input layout
        pd3dDevice->IASetInputLayout(vertexLayout.get());

        // Set primitive topology
        pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

        // Load the Texture
        hr = D3DX10CreateShaderResourceViewFromFile(pd3dDevice, L"seafloor.dds", nullptr, nullptr, &textureRV, nullptr);

        // Initialize the world matrices
        D3DXMatrixIdentity(&world);

        // Initialize the view matrix
        D3DXVECTOR3 Eye(0.0f, 50.0f, -50.0f);
        D3DXVECTOR3 At(0.0f, 0.0f, 0.0f);
        camera.SetViewParams(&Eye, &At);

        return S_OK;
    }


    HRESULT TDXScene::MsgPrc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return camera.HandleMessages(hWnd, uMsg, wParam, lParam);
    }


    HRESULT TDXScene::OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
    {
        // Update the camera's position based on user input 
        camera.FrameMove(fElapsedTime);

        // Modify the color
        meshColor.x = (std::sinf(static_cast<float>(fTime)* 1.0f) + 1.0f) * 0.5f;
        meshColor.y = (std::cosf(static_cast<float>(fTime)* 3.0f) + 1.0f) * 0.5f;
        meshColor.z = (std::sinf(static_cast<float>(fTime)* 5.0f) + 1.0f) * 0.5f;

        return S_OK;
    }


    HRESULT TDXScene::OnRender(ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
    {
        //
        // Clear the back buffer
        //
        auto const ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f }; // red, green, blue, alpha
        auto pRTV = DXUTGetD3D10RenderTargetView();
        pd3dDevice->ClearRenderTargetView(pRTV, ClearColor.begin());

        //
        // Clear the depth stencil
        //
        auto pDSV = DXUTGetD3D10DepthStencilView();
        pd3dDevice->ClearDepthStencilView(pDSV, D3D10_CLEAR_DEPTH, 1.0, 0);

        //
        // Update variables that change once per frame
        //
        auto mat = camera.GetProjMatrix();
        projectionVariable->SetMatrix(reinterpret_cast<float *>(const_cast<D3DXMATRIX *>(mat)));
        mat = camera.GetViewMatrix();
        viewVariable->SetMatrix(reinterpret_cast<float *>(const_cast<D3DXMATRIX *>(mat)));

        worldVariable->SetMatrix(reinterpret_cast<float *>(&world));

        meshColorVariable->SetFloatVector(static_cast<float *>(meshColor));

        //
        // Render the cube
        //
        D3D10_TECHNIQUE_DESC techDesc;
        technique->GetDesc(&techDesc);
        for (auto p = 0U; p < techDesc.Passes; ++p)
        {
            technique->GetPassByIndex(p)->Apply(0);
            pd3dDevice->Draw(N, 0);
        }

        return S_OK;
    }


    HRESULT TDXScene::OnResize(ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
        const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
    {
        // Setup the projection parameters again
        auto const fAspect = static_cast<float>(pBackBufferSurfaceDesc->Width) / static_cast<float>(pBackBufferSurfaceDesc->Height);

        camera.SetProjParams(D3DX_PI / 4, fAspect, 0.1f, 100.0f);
        camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
        camera.SetButtonMasks(MOUSE_MIDDLE_BUTTON, MOUSE_WHEEL, MOUSE_LEFT_BUTTON);

        return S_OK;
    }


    HRESULT TDXScene::RedrawFunc(std::int32_t m, ID3D10Device * pd3dDevice, TDXScene::Re_Im_type reim)
    {
        if (redraw_) {
            th_.reset();
            th_ = std::make_shared<std::thread>([this, m, reim]{ ClearFillSimpleVertex2(m, reim); }, [](std::thread * pth)
            {
                if (pth->joinable()) {
                    pth->join();
                }
            });
            redraw_ = false;
        }

        D3D10_BUFFER_DESC bd;
        bd.Usage = D3D10_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SimpleVertex2) * N;
        bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        D3D10_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = vertices_.data();

        ID3D10Buffer * vertexBuffertmp;
        if (!utility::v_return(pd3dDevice->CreateBuffer(&bd, &InitData, &vertexBuffertmp))) {
            return S_FALSE;
        }

        // Set vertex buffer
        auto stride = sizeof(SimpleVertex2);
        UINT offset = 0;
        pd3dDevice->IASetVertexBuffers(0, 1, &vertexBuffertmp, &stride, &offset);
        vertexBuffer.reset(vertexBuffertmp);

        return S_OK;
    }


    void TDXScene::ClearFillSimpleVertex2(std::int32_t m, TDXScene::Re_Im_type reim)
    {
        SimpleVertex2 sv2;
        sv2.Col = { 0.0f, 0.0f, 0.0f, 0.0f };
        sv2.Pos = { 0.0f, 0.0f, 0.0f };
        boost::fill(vertices_, sv2);

        tbb::task_scheduler_init init;

        tbb::parallel_for(
            std::uint32_t(0),
            N,
            std::uint32_t(1),
            [this, m, reim](std::uint32_t i) { FillSimpleVertex2(m, reim, vertices_[i]); },
            tbb::auto_partitioner());

        th_->join();
    }


    void TDXScene::FillSimpleVertex2(std::int32_t m, TDXScene::Re_Im_type reim, SimpleVertex2 & ver)
    {
        auto sign = 0;
        auto p = 0.0, pp = 0.0;
        double x, y, z;

        using namespace myrandom;

        auto const n = static_cast<double>(pgd_->N);
        auto const rmax = (2.3622 * n + 3.3340) * n + 1.3228;

        MyRand mr(-rmax, rmax);
        auto const rmin = pgd_->R_meshmin();
        double max;
        do {
            if (thread_end_) {
                return;
            }
            max = mr.myrand();
        } while (std::fabs(max) < rmin);

        MyRand mr2(pgd_->Funcmin, (*pgd_)(std::fabs(max)));

        while (true) {
            if (thread_end_) {
                return;
            }

            x = mr.myrand();
            y = mr.myrand();
            z = mr.myrand();

            p = mr2.myrand();
            auto const r = std::sqrt(x * x + y * y + z * z);

            switch (pgd_->Rho_wf_type_) {
            case getdata::GetData::Rho_Wf_type::RHO:
            {
                auto const phi = std::acos(x / std::sqrt(x * x + y * y));
                pp = std::abs((*pgd_)(r)* boost::math::spherical_harmonic(pgd_->L, m, std::acos(z / r), phi));
                pp *= pp;
            }
            break;

            case getdata::GetData::Rho_Wf_type::WF:
            {
                auto const phi = std::acos(x / std::sqrt(x * x + y * y));
                double ylm = 0.0;
                switch (reim) {
                case TDXScene::Re_Im_type::REAL:
                    ylm = boost::math::spherical_harmonic_r(pgd_->L, m, std::acos(z / r), phi);
                    break;

                case TDXScene::Re_Im_type::IMAGINARY:
                    ylm = boost::math::spherical_harmonic_i(pgd_->L, m, std::acos(z / r), phi);
                    break;

                default:
                    BOOST_ASSERT(!"何かがおかしい!");
                    break;
                }

                pp = (*pgd_)(r)* ylm;
            }
            break;

            default:
                BOOST_ASSERT(!"何かがおかしい!");
                break;
            }

            sign = (pp > 0.0) - (pp < 0.0);

            if ((std::fabs(pp) >= std::fabs(p)) ||
                (!m && pgd_->Rho_wf_type_ == getdata::GetData::Rho_Wf_type::WF && reim == TDXScene::Re_Im_type::IMAGINARY)) {
                break;
            }
        }

        ver.Pos.x = static_cast<float>(x);
        ver.Pos.y = static_cast<float>(y);
        ver.Pos.z = static_cast<float>(z);

        ver.Col.r = sign > 0 ? 0.8f : 0.0f;
        ver.Col.b = 0.8f;
        ver.Col.g = sign < 0 ? 0.8f : 0.0f;
        ver.Col.a = 1.0f;
    }

    void rewritewithlock(bool & dest, bool source)
    {
        std::mutex mtx;
        {
            std::lock_guard<std::mutex> lock(mtx);
            dest = source;
        }
    }
}
