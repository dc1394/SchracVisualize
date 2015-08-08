/*! \file tdxscene.cpp
    \brief TDXSceneクラスの実装
    Copyright ©  2015 @dc1394 All Rights Reserved.
    This software is released under the BSD 2-Clause License.
*/

#include "DXUT.h"
#include "DXUTmisc.h"
#include "myrandom/myrand.h"
#include "resource.h"
#include "TDXScene.h"
#include <mutex>                                                // for std::mutex
#include <boost/assert.hpp>                                     // for BOOST_ASSERT
#include <boost/cast.hpp>                                       // for boost::numeric_cast
#include <boost/math/special_functions/spherical_harmonic.hpp>  // for boost::math::spherical_harmonic
#include <boost/range/algorithm.hpp>                            // for boost::fill
#include <tbb/parallel_for.h>                                   // for tbb::parallel_for
#include <tbb/partitioner.h>                                    // for tbb::auto_partitioner
#include <tbb/task_scheduler_init.h>                            // for tbb::task_scheduler_init

namespace tdxscene {
	float const TDXScene::MAGNIFICATION = 1.2f;

	TDXScene::TDXScene(std::shared_ptr<getdata::GetData> const & pgd) :
		Complete([this]{ return complete_; }, nullptr),
		Pth([this]{ return std::cref(pth_); }, nullptr),
		Pgd(nullptr, [this](std::shared_ptr<getdata::GetData> const & val) {
			rmax_ = GetRmax(val);
			SetCamera();
			return pgd_ = val;
		}),
		PLayout([this]{ return std::cref(pLyaout_); }, nullptr),
		Redraw(nullptr, [this](bool redraw){ return redraw_ = redraw; }),
		Thread_end(nullptr, [this](bool thread_end){ return RewriteWithLock(thread_end_, thread_end); }),
		Vertexsize([this]{ return vertexsize_; }, [this](std::vector<SimpleVertex2>::size_type size) { return RewriteWithLock(vertexsize_, size); }),
		projectionVariable_(nullptr),
		pgd_(pgd),
		rmax_(GetRmax(pgd)),
		technique_(nullptr),
		vertices_(VERTEXSIZE_FIRST),
		viewVariable_(nullptr),
		worldVariable_(nullptr)
	{
	}


	HRESULT TDXScene::Init(ID3D10Device* pd3dDevice)
	{
		// Read the D3DX effect_ file
		auto dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		// Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

		ID3D10Effect * effect_tmp;

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
			&effect_tmp,
			nullptr,
			nullptr);

		effect_.reset(effect_tmp);

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

		technique_ = effect_->GetTechniqueByName("Render2");
		worldVariable_ = effect_->GetVariableByName("World")->AsMatrix();
		viewVariable_ = effect_->GetVariableByName("View")->AsMatrix();
		projectionVariable_ = effect_->GetVariableByName("Projection")->AsMatrix();

		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		auto const numElements = sizeof(layout) / sizeof(layout[0]);

		// Create the input layout
		D3D10_PASS_DESC PassDesc;
		technique_->GetPassByIndex(0)->GetDesc(&PassDesc);
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

		pLyaout_.reset(pVertexLayout, utility::Safe_Release<ID3D10InputLayout>());

		// Set the input layout
		pd3dDevice->IASetInputLayout(pLyaout_.get());

		bd_.Usage = D3D10_USAGE_DEFAULT;
		bd_.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		bd_.CPUAccessFlags = 0;
		bd_.MiscFlags = 0;

		// Initialize the world matrices
		D3DXMatrixIdentity(&world_);

		// Initialize the view matrix
		SetCamera();

		return S_OK;
	}


	LRESULT TDXScene::MsgPrc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return camera_.HandleMessages(hWnd, uMsg, wParam, lParam);
	}


	HRESULT TDXScene::OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
	{
		// Update the camera_'s position based on user input 
		camera_.FrameMove(fElapsedTime);

		return S_OK;
	}


	HRESULT TDXScene::OnRender(ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
	{
		//
		// Clear the back buffer
		//
		auto const ClearColor = { 0.0f, 0.0f, 0.0f, 0.0f }; // red, green, blue, alpha
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
		auto matcamera = camera_.GetProjMatrix();
		projectionVariable_->SetMatrix(reinterpret_cast<float *>(const_cast<D3DXMATRIX *>(matcamera)));
		auto matview = camera_.GetViewMatrix();
		viewVariable_->SetMatrix(reinterpret_cast<float *>(const_cast<D3DXMATRIX *>(matview)));

		worldVariable_->SetMatrix(reinterpret_cast<float *>(&world_));

		//
		// Render the cube
		//
		D3D10_TECHNIQUE_DESC techDesc;
		technique_->GetDesc(&techDesc);
		for (auto p = 0U; p < techDesc.Passes; ++p)
		{
			technique_->GetPassByIndex(p)->Apply(0);
			pd3dDevice->Draw(vertexsize_, 0);
		}

		return S_OK;
	}


	HRESULT TDXScene::OnResize(ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
		const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
	{
		// Setup the projection parameters again
		auto const fAspect = static_cast<float>(pBackBufferSurfaceDesc->Width) / static_cast<float>(pBackBufferSurfaceDesc->Height);

		camera_.SetProjParams(D3DX_PI / 4, fAspect, 0.1f, 100.0f);
		camera_.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
		camera_.SetButtonMasks(MOUSE_MIDDLE_BUTTON, MOUSE_WHEEL, MOUSE_LEFT_BUTTON);

		return S_OK;
	}


	HRESULT TDXScene::RedrawFunc(std::int32_t m, ID3D10Device * pd3dDevice, TDXScene::Re_Im_type reim)
	{
		if (redraw_) {
			if (vertices_.size() != vertexsize_) {
				vertices_.resize(vertexsize_);
			}

			pth_.reset(new std::thread([this, m, reim]{ ClearFillSimpleVertex2(m, reim); }), [this](std::thread * pth)
			{
				if (pth->joinable()) {
					RewriteWithLock(thread_end_, true);
					pth->join();
				}

				utility::Safe_Delete<std::thread> sd;
				sd(pth);
			});
			redraw_ = false;
		}

		bd_.ByteWidth = sizeof(SimpleVertex2) * vertexsize_;

		static D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices_.data();

		ID3D10Buffer * vertexBuffertmp;
		if (!utility::v_return(pd3dDevice->CreateBuffer(&bd_, &InitData, &vertexBuffertmp))) {
			return S_FALSE;
		}

		// Set vertex buffer
		static auto const stride = static_cast<UINT>(sizeof(SimpleVertex2));
		static auto const offset = 0U;
		pd3dDevice->IASetVertexBuffers(0, 1, &vertexBuffertmp, &stride, &offset);
		pVertexBuffer_.reset(vertexBuffertmp);

		return S_OK;
	}


	void TDXScene::ClearFillSimpleVertex2(std::int32_t m, TDXScene::Re_Im_type reim)
	{
		RewriteWithLock(complete_, false);

		SimpleVertex2 sv2;
		sv2.Col = { 0.0f, 0.0f, 0.0f, 0.0f };
		sv2.Pos = { 0.0f, 0.0f, 0.0f };
		boost::fill(vertices_, sv2);

		tbb::task_scheduler_init init;

		tbb::parallel_for(
			std::uint32_t(0),
			boost::numeric_cast<std::uint32_t>(vertexsize_),
			std::uint32_t(1),
			[this, m, reim](std::uint32_t i) { FillSimpleVertex2(m, reim, vertices_[i]); },
			tbb::auto_partitioner());

		RewriteWithLock(complete_, true);
	}


	void TDXScene::FillSimpleVertex2(std::int32_t m, TDXScene::Re_Im_type reim, SimpleVertex2 & ver)
	{
		if (thread_end_) {
			return;
		}

		auto pp = 0.0, p = 0.0;
		auto sign = 0;
		double x, y, z;

		myrandom::MyRand mr(-rmax_, rmax_);
		myrandom::MyRand mr2(pgd_->Funcmin, pgd_->Funcmax);

		do {
			if (thread_end_) {
				return;
			}

			x = mr.myrand();
			y = mr.myrand();
			z = mr.myrand();

			auto const r = std::sqrt(x * x + y * y + z * z);
			if (r < pgd_->R_meshmin()) {
				continue;
			}

			switch (pgd_->Rho_wf_type_) {
			case getdata::GetData::Rho_Wf_type::RHO:
			{
				auto const phi = std::acos(x / std::sqrt(x * x + y * y));
                if (m >= 0) {
                    pp = boost::math::spherical_harmonic_r(pgd_->L, m, std::acos(z / r), phi);
                }
                else {
                    pp = boost::math::spherical_harmonic_i(pgd_->L, m, std::acos(z / r), phi);
                }
                
                pp = ((*pgd_)(r)* pp * pp);
				p = mr2.myrand();
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

				pp = (*pgd_)(r) * ylm;
				p = mr2.myrand();
			}
			break;

			default:
				BOOST_ASSERT(!"何かがおかしい!");
				break;
			}

			switch (pgd_->Rho_wf_type_) {
			case getdata::GetData::Rho_Wf_type::RHO:
				sign = -1;
				break;

			case getdata::GetData::Rho_Wf_type::WF:
				sign = (pp > 0.0) - (pp < 0.0);
				break;

			default:
				BOOST_ASSERT(!"何かがおかしい!");
				break;
			}

			if (!m && pgd_->Rho_wf_type_ == getdata::GetData::Rho_Wf_type::WF && reim == TDXScene::Re_Im_type::IMAGINARY) {
				break;
			}
			
		} while (std::fabs(pp) < std::fabs(p));

		ver.Pos.x = static_cast<float>(x);
		ver.Pos.y = static_cast<float>(y);
		ver.Pos.z = static_cast<float>(z);

		ver.Col.r = sign > 0 ? 0.8f : 0.0f;
		ver.Col.b = 0.8f;
		ver.Col.g = sign < 0 ? 0.8f : 0.0f;
		ver.Col.a = 1.0f;
	}


	void TDXScene::SetCamera()
	{
		// Initialize the view matrix
		auto const pos = static_cast<float>(rmax_)* TDXScene::MAGNIFICATION;
		D3DXVECTOR3 Eye(0.0f, pos, -pos);
		D3DXVECTOR3 At(0.0f, 0.0f, 0.0f);
		camera_.SetViewParams(&Eye, &At);
	}


	double GetRmax(std::shared_ptr<getdata::GetData> const & pgd)
	{
		auto const n = static_cast<double>(pgd->N);
		return (2.3622 * n + 3.3340) * n + 1.3228;
	}
}