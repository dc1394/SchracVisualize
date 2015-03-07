﻿/*! \file readdatafile.cpp
    \brief 電子密度のデータファイルを読み込むクラスの実装

    Copyright © 2015 @dc1394 All Rights Reserved.
*/

#include "DXUT.h"
#include "readdatafile.h"
#include <array>                        // for std::array
#include <fstream>                      // for std::ifstream
#include <stdexcept>                    // for std::runtime_error
#include <boost/algorithm/string.hpp>   // for boost::algorithm

namespace getrmeshandrho {
    ReadDataFile::mypair ReadDataFile::readdatafile(std::string const & filename) const
    {
        std::ifstream ifs(filename);
        std::array<char, BUFSIZE> buf;
        std::vector<double> r_mesh, rho;

        // トークン分割
        std::vector<std::string> tokens;
        
        for (auto i = 0;; i++) {
            using namespace boost::algorithm;

            ifs.getline(buf.data(), BUFSIZE);
            std::string line(buf.data());

            split(tokens, line, is_any_of(","), token_compress_on);
                        
            // もし一文字も読めなかったら
            if (!ifs.gcount() && !i) {
                throw std::runtime_error("電子密度のデータファイルが空です！");
            }
            else if (!ifs.gcount()) {
                r_mesh.shrink_to_fit();
                rho.shrink_to_fit();

                return std::make_pair(r_mesh, rho);
            }
            else if (tokens.size() != 2) {
                throw std::runtime_error("電子密度のデータファイルが異常です！");
            }

            std::size_t index1, index2;

            r_mesh.push_back(std::stod(tokens[0], &index1));
            rho.push_back(std::stod(tokens[1], &index2));

            if (tokens[0].size() != index1 || tokens[1].size() != index2) {
                throw std::runtime_error("電子密度のデータファイルが異常です！");
            }
        }
    }
}