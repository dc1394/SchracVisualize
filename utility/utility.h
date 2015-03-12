/*! \file utility.h
    \brief ユーティリティ関数の宣言と実装

    Copyright ©  2015 @dc1394 All Rights Reserved.
*/

#ifndef _UTILITY_H_
#define _UTILITY_H_

#pragma once

#include <memory>               // for std::unique_ptr
#include <string>               // for std::wstring
#include <boost/cast.hpp>       // for boost::numeric_cast
#include <boost/optional.hpp>   // for boost::optional
#include <boost/utility.hpp>    // for boost::checked_delete

namespace utility {
    template <typename T>
    //! A function.
    /*!
        関数が成功したかどうかを判断する
        \param x HRESULTの値
        \return 成功したらboost::optional<HRESULT>、失敗したらboost::none
    */
    boost::optional<HRESULT> v_return(T const & x);

    template <typename T>
    //! A struct.
    /*!
        リソースを安全に解放するクラス
    */
    struct Safe_Release final {
        //! A public member function.
        /*!
            リソースを安全に解放する
            \param p リソースへのポインタ
        */
        void operator()(T * p) {
            if (p) {
                p->Release();
                p = nullptr;
            }
        }
    };

    template <typename T>
    //! A function.
    /*!
        確保したメモリを安全に解放するクラス
    */
    struct Safe_Delete final {
        //! A public member function.
        /*!
            確保したメモリを安全に解放する
            \param p 確保したメモリの先頭アドレス
        */
        void operator()(T * p) {
            if (p) {
                boost::checked_delete(p);
                p = nullptr;
            }
        }
    };

    template <typename T> T v_return(T const & x)
    {
        auto const hr = boost::numeric_cast<HRESULT>(x);
        return hr >= 0 ? boost::optional<HRESULT>(hr) : boost::none;
    }

    template <class T, class D>
    std::unique_ptr<T, D> makeUniqueWithDeleter(T * t, D deleter)
    {
        std::unique_ptr<T, D> ptr(t, deleter);

        return ptr;
    }
}

#endif  // _UTILITY_H_
