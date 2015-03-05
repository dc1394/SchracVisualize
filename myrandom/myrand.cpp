/*! \file myrand.h
    \brief 自作乱数クラスの実装

    Copyright © 2015 @dc1394 All Rights Reserved.
*/

#include "DXUT.h"
#include "myrand.h"
#include <boost/range/algorithm.hpp>    // for boost::generate

namespace myrandom {
    MyRand::MyRand() :
        distribution_(0.0f, 1.0f)
    {
        // ランダムデバイス
        std::random_device rnd;

        // 初期化用ベクタ
        std::vector<std::uint_least32_t> v(SIZE);

        // ベクタの初期化
        boost::generate(v, std::ref(rnd));
        
        // 乱数エンジン
        randengine_ = std::mt19937(std::seed_seq(v.begin(), v.end()));
    }
}
