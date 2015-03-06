/*! \file readdatafile.cpp
    \brief 動径波動関数ファイルを読み込むクラスの実装

    Copyright © 2015 @dc1394 All Rights Reserved.
*/

#include "readdatafile.h"
#include <array>            // for std::array
#include <fstream>          // for std::ifstream

namespace readdatafile {
    std::pair<ReadDataFile::dvector, ReadDataFile::dvector> ReadDataFile::readdatafile(std::wstring const & filename) const
    {
        std::ifstream ifs(filename);
        std::array<char, BUFSIZE> buf;
        ifs.getline(buf.data(), BUFSIZE);
        std::string line(buf.data());

        // もし一文字も読めなかったら
        if (!ifs.gcount()) {
            
        }
    }
}