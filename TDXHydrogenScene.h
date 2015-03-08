#pragma once
#include "tdxscene.h"
#include "DXUTcamera.h"

#include "getrmeshandrho/getrmeshandrho.h"

#include "WaveFunctions.h"

#include <array>        // for std::array
#include <cstdint>      // for std::int32_t
#include <string>       // for std::string
#include <d3dx9math.h>
//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
#ifndef	SIMPLEVER2
struct SimpleVertex2
{
    D3DXVECTOR3 Pos;
    D3DXCOLOR	Col;
};
#define SIMPLEVER2
#endif



#define DEG2RAD( a ) ( a * D3DX_PI / 180.f )

class TDXHydrogenScene :
	public TDXScene
{
public:
	TDXHydrogenScene(void);
	virtual ~TDXHydrogenScene(void);
  virtual HRESULT Init(ID3D10Device* pd3dDevice);
  virtual HRESULT OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
  virtual HRESULT OnRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
  virtual HRESULT OnResize( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
  virtual HRESULT MsgPrc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
ID3D10Effect*                       g_pEffect; //エフェクト＝シェーダプログラムを読ませるところ
ID3D10InputLayout*                  g_pVertexLayout;
ID3D10EffectTechnique*              g_pTechnique;
ID3D10Buffer*                       g_pVertexBuffer;
ID3D10Buffer*                       g_pIndexBuffer;
ID3D10ShaderResourceView*           g_pTextureRV;
ID3D10EffectMatrixVariable*         g_pWorldVariable;
ID3D10EffectMatrixVariable*         g_pViewVariable;
ID3D10EffectMatrixVariable*         g_pProjectionVariable;
ID3D10EffectVectorVariable*         g_pMeshColorVariable;
ID3D10EffectVectorVariable*		    g_pLightDirVariable;
ID3D10EffectShaderResourceVariable* g_pDiffuseVariable;
D3DXMATRIX                          g_World;
D3DXMATRIX                          g_View;
D3DXMATRIX                          g_Projection;
D3DXVECTOR4                         g_vMeshColor;
D3DXVECTOR4							g_vLightDir;
CModelViewerCamera                  g_Camera;               // A model viewing camera

static	const unsigned int N=20000;

private:
    getrmeshandrho::GetRMeshAndRho const grmar_;

};

std::string MyOpenFile();
BOOL ShowFileDialog(HWND hWnd, wchar_t * filepath, wchar_t * filename, wchar_t const * title, wchar_t const * deffile);
std::string my_wcstombs(std::array<wchar_t, MAX_PATH> const & wcs, std::int32_t codeMulti = CP_ACP);