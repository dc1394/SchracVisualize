/*! \file tdxscene.h
    \brief TDXSceneクラスの宣言

    Copyright ©  2015 @dc1394 All Rights Reserved.
*/

#ifndef _TDXSCENE_H_
#define _TDXSCENE_H_

#pragma once

#include "DXUT.h"
#include "DXUTcamera.h"
#include "getdata/getdata.h"
#include "utility/utility.h"
#include <memory>               // for std::shared_ptr, for std::unique_ptr
#include <vector>               // for std::vector
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

class TDXScene final {
    // #region コンストラクタ・デストラクタ

public:
    //! A constructor.
    /*!
        唯一のコンストラクタ
    */
    TDXScene(std::shared_ptr<getdata::GetData> const & pgd);

    //! A destructor.
    /*!
        デストラクタ
    */
    ~TDXScene() = default;
	
    // #endregion コンストラクタ・デストラクタ

    // #region メンバ関数
	
public:
    static void operator delete(void* p, void* buf){}

    HRESULT Init(ID3D10Device* pd3dDevice);
    HRESULT OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
    HRESULT OnRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
    HRESULT OnResize( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                    const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
    HRESULT MsgPrc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    // ! A public member function.
    /*!
        再描画する
        \param pd3dDevice DirectXデバイスへのスマートポインタ
        \return 再描画が成功したかどうか
    */
    HRESULT Redraw(ID3D10Device * pd3dDevice);

private:
    // ! A private member function.
    /*!
        SimpleVertex2にデータを詰める
        \param ver 対象のSimpleVertex2
    */
    void FillSimpleVertex2(SimpleVertex2 & ver);
    
    // #endregion メンバ関数

    // #region メンバ変数

    // ! A private static member variable (constant).
    /*!
        電子のサンプル点
    */
    static std::size_t const N = 80000;
    
    // ! A private member variable.
    /*!
        A model viewing camera
    */
    CModelViewerCamera                  camera;

    // ! A private member variable.
    /*!
        エフェクト＝シェーダプログラムを読ませるところ
    */
    std::unique_ptr<ID3D10Effect, utility::Safe_Release<ID3D10Effect>> effect;

    ID3D10EffectShaderResourceVariable* diffuseVariable;

    D3DXVECTOR4							lightDir;

    ID3D10EffectVectorVariable*		    lightDirVariable;
    
    // ! A private member variable.
    /*!
        メッシュの色
    */
    D3DXVECTOR4                         meshColor;

    ID3D10EffectVectorVariable*         meshColorVariable;
    
    // ! A private member variable.
    /*!
        射影行列
    */
    D3DXMATRIX                          projection;


    ID3D10EffectMatrixVariable*         projectionVariable;

    // ! A private member variable (constant).
    /*!
        rのメッシュとデータ
    */
    std::shared_ptr<getdata::GetData> const pgd_;


    ID3D10EffectTechnique*              technique;

    std::unique_ptr<ID3D10ShaderResourceView, utility::Safe_Release<ID3D10ShaderResourceView>> textureRV;
    
    std::unique_ptr<ID3D10Buffer, utility::Safe_Release<ID3D10Buffer>> vertexBuffer;

    std::unique_ptr<ID3D10InputLayout, utility::Safe_Release<ID3D10InputLayout>> vertexLayout;
    
    // ! A private member variable.
    /*!
        vertex buffer
    */
    std::vector<SimpleVertex2> vertices;
    
    // ! A private member variable.
    /*!
        ビュー変換行列
    */
    D3DXMATRIX                          view;

    ID3D10EffectMatrixVariable*         viewVariable;
    
    // ! A private member variable.
    /*!
        ワールド変換行列
    */
    D3DXMATRIX                          world;
        
    ID3D10EffectMatrixVariable*         worldVariable;
        
    // #region 禁止されたコンストラクタ・メンバ関数
    
    //! A private constructor (deleted).
    /*!
        デフォルトコンストラクタ（禁止）
    */
    TDXScene() = delete;

    //! A private copy constructor (deleted).
    /*!
        コピーコンストラクタ（禁止）
    */
    TDXScene(TDXScene const &) = delete;

    //! A private member function (deleted).
    /*!
        operator=()の宣言（禁止）
        \param コピー元のオブジェクト（未使用）
        \return コピー元のオブジェクト
    */
    TDXScene & operator=(TDXScene const &) = delete;

    // #endregion 禁止されたコンストラクタ・メンバ関数
};

#endif  // _TDXSCENE_H_
