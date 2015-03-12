#pragma once

#include "DXUTcamera.h"
#include "getdata/getdata.h"
#include "tdxscene.h"
#include "utility/utility.h"
#include <array>        // for std::array
#include <cstdint>      // for std::int32_t
#include <memory>       // for std::shared_ptr, for std::unique_ptr
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

class TDXHydrogenScene final :
	public TDXScene
{
    // #region コンストラクタ・デストラクタ

public:
    //! A constructor.
    /*!
        唯一のコンストラクタ
    */
    TDXHydrogenScene(std::shared_ptr<getdata::GetData> const & pgd);

    //! A destructor.
    /*!
        デストラクタ
    */
    virtual ~TDXHydrogenScene(void);
	
    // #endregion コンストラクタ・デストラクタ

    // #region メンバ関数
	
public:
    HRESULT Init(ID3D10Device* pd3dDevice) override;
    HRESULT OnFrameMove( double fTime, float fElapsedTime, void* pUserContext ) override;
    HRESULT OnRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext ) override;
    HRESULT OnResize( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                    const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext ) override;
    HRESULT MsgPrc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) override;

    // ! A public member function.
    /*!
        再描画する
        \param pd3dDevice DirectXデバイスへのスマートポインタ
        \return 再描画が成功したかどうか
    */
    HRESULT Redraw(ID3D10Device * pd3dDevice) override;

    // ! A private member function.
    /*!
        SimpleVertex2にデータを詰める
        \param ver 対象のSimpleVertex2
    */
    void FillSimpleVertex2(SimpleVertex2 & ver);



    // #endregion メンバ関数

    // #region メンバ変数

private:
    // ! A private static member variable (constant).
    /*!
        電子のサンプル点
    */
    static std::size_t const N = 80000;

protected:
    // ! A protected member variable.
    /*!
        エフェクト＝シェーダプログラムを読ませるところ
    */
    
    std::unique_ptr<ID3D10Effect, utility::Safe_Release<ID3D10Effect>> g_pEffect;
    ID3D10InputLayout*                  g_pVertexLayout;
    ID3D10EffectTechnique*              g_pTechnique;
    ID3D10Buffer*                       g_pVertexBuffer;
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

    // ! A protected member variable.
    /*!
        A model viewing camera
    */
    CModelViewerCamera                  g_Camera;
    
private:
    // ! A private member variable (constant).
    /*!
        rのメッシュと電子密度
    */
    std::shared_ptr<getdata::GetData> const pgd_;
};

// #region 非メンバ関数

// ! A function.
/*!
    マルチバイト文字列をワイド文字列に変換する
    \param mbs マルチバイト文字列
    \param codeMulti マルチバイト文字列のエンコード（デフォルト=932）
    \return ワイド文字列
*/
std::wstring my_mbstowcs(std::string const & mbs, std::int32_t codeMulti = 932);

// ! A function.
/*!
    ファイルを開く
    \return ファイル名
*/
std::string MyOpenFile();

// ! A function.
/*!
    ワイド文字列をマルチバイト文字列に変換する
    \param wcs ワイド文字列
    \param codeMulti マルチバイト文字列のエンコード（デフォルト=932）
    \return マルチバイト文字列
*/
std::string my_wcstombs(std::array<wchar_t, MAX_PATH> const & wcs, std::int32_t codeMulti = 932);

// ! A function.
/*!
    ファイル選択ダイアログを開く
    \param hWnd ウィンドウハンドル
    \param filepath ファイルのパス
    \param filename ファイル名
    \param title ファイル選択ダイアログのタイトル
    \param defextension デフォルトのファイルの種類
    \return ファイル選択ダイアログの戻り値
*/
BOOL ShowFileDialog(HWND hWnd, wchar_t * filepath, wchar_t * filename, wchar_t * title, wchar_t * defextension);

// #endregion 非メンバ関数