/*! \file tdxscene.h
    \brief TDXSceneクラスの宣言

    Copyright ©  2015 @dc1394 All Rights Reserved.
    This software is released under the BSD-2 License.
*/

#ifndef _TDXSCENE_H_
#define _TDXSCENE_H_

#pragma once

#include "DXUT.h"
#include "DXUTcamera.h"
#include "getdata/getdata.h"
#include "utility/property.h"
#include "utility/utility.h"
#include <memory>               // for std::shared_ptr, for std::unique_ptr
#include <thread>               // for std::thread
#include <vector>               // for std::vector
#include <d3dx9math.h>

namespace tdxscene {
	//! A class.
	/*!
	画面描画クラス
	*/
	class TDXScene final {
	public:
		// #region 構造体

#ifndef	SIMPLEVER2
		//! A struct.
		/*!
		頂点構造体
		*/
		struct SimpleVertex2 {
			//! A public member variable.
			/*!
			位置
			*/
			D3DXVECTOR3 Pos;

			//! A public member variable.
			/*!
			色
			*/
			D3DXCOLOR	Col;
		};
#define SIMPLEVER2
#endif

		// #endregion 構造体

		// #region 列挙型

		//! A enumerated type
		/*!
		実部か虚部かを表す列挙型
		*/
		enum class Re_Im_type {
			// 実部
			REAL,
			// 虚部
			IMAGINARY
		};

		// #endregion 列挙型

		// #region コンストラクタ・デストラクタ

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

		//! A public member function.
		/*!
		Direct3Dデバイスを初期化する
		\param Direct3Dデバイスへのポインタ
		\return 初期化が成功したかどうか
		*/
		HRESULT Init(ID3D10Device* pd3dDevice);

		LRESULT MsgPrc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		HRESULT OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
		HRESULT OnRender(ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext);
		HRESULT OnResize(ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
			const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);

		//! A public member function.
		/*!
		再描画する
		\param m 磁気量子数
		\param pd3dDevice DirectXデバイスへのスマートポインタ
		\param reim 実部を描画するか、虚部を描画するか
		\return 再描画が成功したかどうか
		*/
		HRESULT RedrawFunc(std::int32_t m, ID3D10Device * pd3dDevice, TDXScene::Re_Im_type reim);

	private:
		//! A private member function.
		/*!
		SimpleVertex2のデータをクリアし、新しいデータを詰める
		\param m 磁気量子数
		\param reim 実部を描画するか、虚部を描画するか
		\param ver 対象のSimpleVertex2
		*/
		void ClearFillSimpleVertex2(std::int32_t m, TDXScene::Re_Im_type reim);

		//! A private member function.
		/*!
		SimpleVertex2にデータを詰める
		\param m 磁気量子数
		\param reim 実部を描画するか、虚部を描画するか
		\param ver 対象のSimpleVertex2
		*/
		void FillSimpleVertex2(std::int32_t m, TDXScene::Re_Im_type reim, SimpleVertex2 & ver);

		template <typename FUNCTYPE>
		//! A private member function (template function).
		/*!
		データオブジェクトからrmaxを求める
		\param func 最大最小を求めるための関数ポインタ
		\return 関数の最大値
		*/
		double FuncMinMax(utility::Functional<FUNCTYPE> const & func) const;

		//! A private member function.
		/*!
		カメラの位置をセットする
		*/
		void SetCamera();

		// #endregion メンバ関数

		// #region プロパティ

	public:
		//! A property.
		/*!
		描画スレッドの作業が完了したかどうかへのプロパティ
		*/
		utility::Property<bool> const Complete;

		//! A property.
		/*!
		スレッドへのスマートポインタのプロパティ
		*/
		utility::Property<std::shared_ptr<std::thread>> const Pth;

		//! A property.
		/*!
		データオブジェクトのスマートポインタのプロパティ
		*/
		utility::Property<std::shared_ptr<getdata::GetData>> Pgd;

		//! A property.
		/*!
		VertexLayoutのスマートポインタのプロパティ
		*/
		utility::Property<std::shared_ptr<ID3D10InputLayout>> const PvertexLayout;

		//! A property.
		/*!
		再描画するかどうかへのプロパティ
		*/
		utility::Property<bool> Redraw;

		//! A property.
		/*!
		スレッドを強制終了するかどうかへのプロパティ
		*/
		utility::Property<bool> Thread_end;

		//! A property.
		/*!
		頂点数へのプロパティ
		*/
		utility::Property<std::vector<SimpleVertex2>::size_type> Vertexsize;

		// #endregion プロパティ

		// #region メンバ変数

	public:
		//! A public static member variable (constant).
		/*!
		頂点の初期値
		*/
		static std::vector<SimpleVertex2>::size_type const VERTEXSIZE_FIRST = 10000;

	private:
		//! A private static member variable (constant).
		/*!
		頂点の初期値
		*/
		static float const MAGNIFICATION;

		//! A private member variable.
		/*!
		バッファー リソース
		*/
		D3D10_BUFFER_DESC bd_;

		//! A private member variable.
		/*!
		A model viewing camera
		*/
		CModelViewerCamera camera_;

		//! A private member variable.
		/*!
		描画スレッドの作業が完了したかどうか
		*/
		bool complete_;

		//! A private member variable.
		/*!
		エフェクト＝シェーダプログラムを読ませるところ
		*/
		std::unique_ptr<ID3D10Effect, utility::Safe_Release<ID3D10Effect>> effect_;

		//! A private member variable.
		/*!
		射影行列
		*/
		D3DXMATRIX projection_;

		ID3D10EffectMatrixVariable * projectionVariable_;

		//! A private member variable.
		/*!
		スレッドへのスマートポインタ
		*/
		std::shared_ptr<std::thread> pth_;

		//! A private member variable.
		/*!
		rのメッシュとデータ
		*/
		std::shared_ptr<getdata::GetData> pgd_;

		//! A private member variable.
		/*!
		再描画するかどうか
		*/
		bool redraw_ = true;

		//! A private member variable.
		/*!
		描画するrの最大値
		*/
		double rmax_;

		double thetamax;

		//! A private member variable.
		/*!
		テクニック情報
		*/
		ID3D10EffectTechnique * technique_;

		//! A private member variable.
		/*!
		スレッドを強制終了するかどうか
		*/
		bool thread_end_ = false;

		//! A private member variable.
		/*!
		頂点バッファ
		*/
		std::unique_ptr<ID3D10Buffer, utility::Safe_Release<ID3D10Buffer>> vertexBuffer_;

		//! A private member variable.
		/*!
		入力レイアウト インターフェイス
		*/
		std::shared_ptr<ID3D10InputLayout> vertexLayout_;

		//! A private member variable.
		/*!
			頂点数
		*/
		std::vector<SimpleVertex2>::size_type vertexsize_ = VERTEXSIZE_FIRST;

		//! A private member variable.
		/*!
			vertex buffer
		*/
		std::vector<SimpleVertex2> vertices_;

		//! A private member variable.
		/*!
			ビュー変換行列
		*/
		D3DXMATRIX view_;

		ID3D10EffectMatrixVariable * viewVariable_;

		//! A private member variable.
		/*!
			ワールド変換行列
		*/
		D3DXMATRIX world_;

		ID3D10EffectMatrixVariable * worldVariable_;

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

    //! A function.
    /*!
        データオブジェクトからrmaxを求める
        \param pgd データオブジェクト
        \return rmaxの値
    */
    double GetRmax(std::shared_ptr<getdata::GetData> const & pgd);

	template <typename T>
	//! A template function.
    /*!
        ロックをかけて変数を書き換える関数
        \param dest 対象パラメータへの参照
        \param source 値
        \return 書き換えた値
    */
    T RewriteWithLock(T & dest, T source)
    {
        std::mutex mtx;
        {
            std::lock_guard<std::mutex> lock(mtx);
            dest = source;
        }

        return dest;
    }
}

#endif  // _TDXSCENE_H_
