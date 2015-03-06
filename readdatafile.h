/*! \file readdatafile.h
    \brief 動径波動関数ファイルを読み込むクラスの宣言

    Copyright © 2015 @dc1394 All Rights Reserved.
*/

#ifndef _READDATAFILE_H_
#define _READDATAFILE_H_

#pragma once

#include <string>   // for std::wstring
#include <utility>  // for std::pair
#include <vector>   // for std::vector

namespace readdatafile {
    //! A class.
    /*!
        動径波動関数ファイルを読み込むクラス
    */
    struct ReadDataFile final {
        // #region 型エイリアス

        using dvector = std::vector < double >;

        // #endregion 型エイリアス

        // #region コンストラクタ・デストラクタ

        //! A constructor.
        /*!
            唯一のコンストラクタ
        */
        ReadDataFile() = default;

        //! A destructor.
        /*!
        デフォルトデストラクタ
        */
        ~ReadDataFile() = default;

        // #endregion コンストラクタ・デストラクタ

        // #region メンバ関数

        //!  A public member function.
        /*!
            実際にデータファイルを読み込む
        */
        std::pair<dvector, dvector> readdatafile(std::wstring const & filename) const;

        // #endregion メンバ関数

        // #region メンバ変数

    private:
        // !A private member variable(constant expression).
        /*!
            バッファサイズ
        */
        static constexpr std::streamsize BUFSIZE = 1024;

        // #endregion メンバ変数
    }
}

#endif  // _READDATAFILE_H_
