/*! \file myrand.h
    \brief 自作乱数クラスの宣言

    Copyright © 2015 @dc1394 All Rights Reserved.
    This software is released under the BSD 2-Clause License.
*/

#ifndef _MYRAND_H_
#define _MYRAND_H_

#pragma once

#include <cstdint>  // for std::uint_least32_t
#include <random>   // for std::mt19937
#include <vector>   // for std::vector

namespace myrandom {
    //! A class.
    /*!
        自作乱数クラス
    */
    class MyRand final {
        // #region コンストラクタ・デストラクタ

    public:
        //! A constructor.
        /*!
            唯一のコンストラクタ
            \param min 乱数分布の最小値
            \param max 乱数分布の最大値
        */
        MyRand(double min, double max);

        //! A destructor.
        /*!
            デフォルトデストラクタ
        */
        ~MyRand() = default;

        // #endregion コンストラクタ・デストラクタ

        // #region メンバ関数

        //!  A public member function.
        /*!
            [0.0, 1.0]の閉区間で一様乱数を生成する
        */
        double myrand()
        {
            return distribution_(randengine_);
        }

        // #endregion メンバ関数

        // #region メンバ変数

    private:
        //! A private static member variable (constant expression).
        /*!
            初期乱数生成用のstd::vectorのサイズ
        */
        static std::vector<std::uint_least32_t>::size_type const SIZE = 64;

        //! A private member variable.
        /*!
            乱数の分布
        */
        std::uniform_real_distribution<double> distribution_;
        
        //! A private member variable.
        /*!
            乱数エンジン
        */
        std::mt19937 randengine_;

        // #region 禁止されたコンストラクタ・メンバ関数

        //! A private constructor (deleted).
        /*!
        デフォルトコンストラクタ（禁止）
        */
        MyRand() = delete;

        //! A private copy constructor (deleted).
        /*!
            コピーコンストラクタ（禁止）
        */
        MyRand(const MyRand &) = delete;

        //! A private member function (deleted).
        /*!
            operator=()の宣言（禁止）
            \param コピー元のオブジェクト（未使用）
            \return コピー元のオブジェクト
        */
        MyRand & operator=(const MyRand &) = delete;

        // #endregion 禁止されたコンストラクタ・メンバ関数
    };
}

#endif  // _MYRAND_H_
