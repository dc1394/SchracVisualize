/*! \file getrmeshandrho.h
    \brief rのメッシュと、そのメッシュにおける電子密度を与えるクラスの宣言

Copyright © 2015 @dc1394 All Rights Reserved.
*/

#ifndef _GETRMESHANDRHO_H_
#define _GETRMESHANDRHO_H_

#pragma once

#include "deleter.h"
#include "property.h"
#include <memory>       // for std::unique_ptr
#include <string>       // for std::string

namespace getrmeshandrho {
    //! A class.
    /*!
        rのメッシュと、そのメッシュにおける動径波動関数を与えるクラス
    */
    class GetRMeshAndRho final {
        // #region コンストラクタ・デストラクタ

    public:
        //! A constructor.
        /*!
            唯一のコンストラクタ
            \param filename rのメッシュと、そのメッシュにおける電子密度が記録されたデータファイル名
        */
        GetRMeshAndRho(std::string const & filename);

        //! A destructor.
        /*!
            デフォルトデストラクタ
        */
        ~GetRMeshAndRho() = default;

        // #endregion コンストラクタ・デストラクタ

        // #region メンバ関数

        //!  A public member function (const).
        /*!
            電子密度ρ(r)を返す
            \param r rの値
            \return ρ(r)の値
        */
        double operator()(double r) const;

        // #endregion メンバ関数
        
        // #region プロパティ

        //! A property.
        /*!
            電子密度ρ(r)の最大値のプロパティ
        */
        Property<double> const Rhomax;

        //! A property.
        /*!
            rのメッシュの最小値のプロパティ
        */
        Property<double> const R_meshmin;
        
        // #endregion プロパティ

        // #region メンバ変数
        
    private:
        //! A private member variable.
        /*!
            gsl_interp_accelへのスマートポインタ
        */
        std::unique_ptr<gsl_interp_accel, decltype(gsl_interp_accel_deleter)> const acc_;

        //!  A public member variable.
        /*!
            電子密度ρ(r)の最大値
        */
        double rhomax_;

        //!  A public member variable.
        /*!
            rのメッシュの最小値
        */
        double r_meshmin_;

        //! A private member variable.
        /*!
            gsl_interp_typeへのスマートポインタ
        */
        std::unique_ptr<gsl_spline, decltype(gsl_spline_deleter)> spline_;

        // #endregion メンバ変数

        // #region 禁止されたコンストラクタ・メンバ関数

        //! A private constructor (deleted).
        /*!
            デフォルトコンストラクタ（禁止）
        */
        GetRMeshAndRho() = delete;

        //! A private copy constructor (deleted).
        /*!
            コピーコンストラクタ（禁止）
        */
        GetRMeshAndRho(GetRMeshAndRho const &) = delete;

        //! A private member function (deleted).
        /*!
            operator=()の宣言（禁止）
            \param コピー元のオブジェクト（未使用）
            \return コピー元のオブジェクト
        */
        GetRMeshAndRho & operator=(GetRMeshAndRho const &) = delete;

        // #endregion 禁止されたコンストラクタ・メンバ関数
    };
}

#endif  // _GETRMESHANDRHO_H_
