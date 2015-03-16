/*! \file xor514.h
    \brief SSE2を使って高速化したXorshiftによる乱数生成クラスの宣言
    
    but this is originally adapted by 和田維作 さま
    cf. http://www001.upp.so-net.ne.jp/isaku/xor.c.html
    
    Copyright ©  2014 @dc1394 All Rights Reserved.
*/

#ifndef _XOR514_H_
#define _XOR514_H_

#pragma once

// 以下はオリジナルのソースコードのコメント
/* xor.c : coded by isaku@pb4.so-net.ne.jp 2008/12～2009/1,  2010/10, 2012/6
乱数 Xorshift のＳＳＥ２を使った高速化(整数版)

xor128 は論文に書いてあった 128 bit Xorshift である。

以前の xor130 は SSE2 を使って、１２８ビット演算を行うものであった。
一度に４個の整数を得るので、周期は 2^{130}-4 であった。
１ビット単位のシフト演算は SSE2 ではサポートされていないので、
他の演算を組み合わせることで１２８ビットシフト演算を実現した。

最初のバージョンでは

enum { a=11,b=23 };
x^=x<<a; x^=x>>b; return x;

のように、二つの Xorshift 演算を行っていた。
Diehard テストを行ったところ、良好な結果を得たが、
Dieharder テストでは必ず不合格が出るので、
次のバージョンでは、三つの Xorshift 演算を行うように改良した。

enum { a=8,b=69,c=75 };
x^=x<<a; x^=x>>b; x^=x<<c; return x;

高速に計算できるパラメータを選んだので、
古いバージョンと同じスピードで実行できた。
古い Dieharder テストで不合格がでなくなったが、
新しい Dieharder テスト(3.31.1)では不合格になってしまった。

そこで、

enum { a=101,b=99,c=8 };
t=x^x<<a; x=y; y=z; z=w; t^=t>>b; w^=w>>c^t; return w;
とした xor514 にしたところ、新しい Dieharder テストでも不合格が出なくなった。
周期は 2^{514}-4 であ

以下のプログラムは xor128 , xor514 のそれぞれについて、
１０億個 × ８の乱数を足し合わせて、最速の実行時間を比較する。

１０億回の生成時間(単位 秒)の結果

Core i7 2600(turbo boost off)
+--------------------------------------------------+--------+--------+
| compiler and system                              | xor128 | xor514 |
+--------------------------------------------------+--------+--------+
| VC++2010 64bit cl /O2 /GL /W4  on Windows7 64bit |  1.856 |  1.326 |
| VC++2010 32bit cl /O2 /GL /W4  on Windows7 64bit |  2.262 |  1.554 |
| gcc -O4 -Wall              on Ubuntu 12.04 64bit |  1.800 |  1.230 |
| gcc -O4 -Wall -m32 -msse2  on Ubuntu 12.04 64bit |  2.320 |  1.040 |
+--------------------------------------------------+--------+--------+
*/

#include <DXUT.h>
#include <cstdint>      // for std::int32_t, std::uint32_t
#include <ctime>        // for std::time
#include <limits>       // for std::numeric_limits<double>::max
#include <emmintrin.h>  // for __m128i

#ifdef __GNUC__
    #define align32 __attribute__((aligned(32)))
#else
    #define align32 _declspec(align(32))
#endif

namespace myrandom {
    //! A class.
    /*!
        SSE2を使って高速化したXorshiftによる乱数生成クラス
    */
    class Dxor156 final {
    private:
        // #region 共用体

        //! A union.
        /*!
            xorshiftのデータが格納される共用体
        */
        union dxor156_t {
            std::int32_t i[4];
            double d[2];
            __m128i m;
        };
        
        //! A union.
        /*!
            xorshiftで補助的に使われる共用体
        */
        union mask_t {
            std::int32_t i[4];
            __m128i m;
        };

        // #endregion 共用体

    public:
        // #region コンストラクタ・デストラクタ

        //! A constructor.
        /*!
            デフォルトコンストラクタ
        */
        Dxor156() : Dxor156(static_cast<std::uint32_t>(std::time(nullptr)))
        {
        }

        //! A constructor.
        /*!
            \param seed 乱数のシード
        */
        explicit Dxor156(std::uint32_t seed);

        //! A destructor.
        /*!
            デストラクタ
        */
        ~Dxor156() = default;
        
        // #endregion コンストラクタ・デストラクタ

        // #region メンバ関数

        //! A public member function.
        /*!
            [xmin, xmax]における乱数を生成する
            \param xmin 乱数の最小値
            \param xmax 乱数の最大値
            \return 乱数
        */
        double rand(double xmin, double xmax);
        
        // #endregion メンバ関数

    private:
        // #region メンバ変数
        
        //! A union.
        /*!
            xorshiftのmask
        */
        mask_t mask_;
        
        //! A union.
        /*!
            xorshiftのデータ
        */
        dxor156_t x_;

        //! A union.
        /*!
            xorshiftのデータ
        */
        dxor156_t y_;

        //! A union.
        /*!
            xorshiftのデータ
        */
        dxor156_t z_;

        // #region 禁止されたコンストラクタ・メンバ関数

        //! A private copy constructor (deleted).
        /*!
            コピーコンストラクタ（禁止）
        */
        Dxor156(Dxor156 const &) = delete;

        //! A private member function (deleted).
        /*!
            operator=()の宣言（禁止）
            \param コピー元のオブジェクト
            \return コピー元のオブジェクト
        */
        Dxor156 & operator=(Dxor156 const &) = delete;

        // #endregion 禁止されたコンストラクタ・メンバ関数
    };

    inline Dxor156::Dxor156(std::uint32_t seed)
    {
        mask_ = { { -1, 0xFFFFF, 0, 0 } };

        x_ = { { 123456789, (362436069 & 0xFFFFF) | 0x3FF00000, 0, 0 } },
        y_ = { { 521288629, (88675123 & 0xFFFFF) | 0x3FF00000, 0, 0 } },
        z_ = { { 5783321, (6615241 & 0xFFFFF) | 0x3FF00000, 0, 0 } };

        /*for (auto i = 1; i <= 4; i++)
        {
            x_.i[i - 1] = seed = 1812433253U * (seed ^ (seed >> 30)) + i;
            y_.i[i - 1] = seed = 1812433253U * (seed ^ (seed >> 30)) + i;
            z_.i[i - 1] = seed = 1812433253U * (seed ^ (seed >> 30)) + i;
        }*/
    }

    inline double Dxor156::rand(double xmin, double xmax)
    {
        auto t = x_.m;
        
        auto s = _mm_slli_epi64(t, 19);
        t = _mm_xor_si128(t, s);
        t = _mm_and_si128(t, mask_.m);
        
        s = _mm_srli_epi64(t, 33);
        s = _mm_xor_si128(s, t);
        
        t = y_.m;
        x_.m = t;
        t = z_.m;
        y_.m = t;
        t = _mm_xor_si128(t, s);
        z_.m = t;

        auto const aa = (z_.d[0] - 1.0);
        return aa * (xmax - xmin) + xmin;
    }
}

#endif  // _XOR514_H_
