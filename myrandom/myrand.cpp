/*! \file myrand.h
    \brief 自作乱数クラスの実装

    Copyright © 2015 @dc1394 All Rights Reserved.
    This software is released under the BSD-2 License.
*/

#include "DXUT.h"
#include "myrand.h"
#include <boost/range/algorithm.hpp>    // for boost::generate

namespace myrandom {
    MyRand::MyRand(double min, double max) :
        distribution_(min, max)
    {
        // ランダムデバイス
        std::random_device rnd;

        // 初期化用ベクタ
        std::vector<std::uint_least32_t> v(SIZE);

        // ベクタの初期化
        boost::generate(v, std::ref(rnd));
        
        std::seed_seq seq(v.begin(), v.end());

        // 乱数エンジン
        randengine_ = std::mt19937(seq);
    }
}
