﻿/*! \file getdata.h
    \brief rのメッシュと、そのメッシュにおける電子密度を与えるクラスの実装

    Copyright © 2015 @dc1394 All Rights Reserved.
*/

#include "DXUT.h"
#include "getdata.h"
#include "readdatafile.h"
#include <stdexcept>                    // for std::runtime_error
#include <tuple>                        // for std::tie
#include <boost/algorithm/string.hpp>   // for boost::algorithm
#include <boost/assert.hpp>             // for BOOST_ASSERT
#include <boost/cast.hpp>               // for boost::cast
#include <boost/range/algorithm.hpp>    // for boost::max_element

namespace getdata {
    // #region コンストラクタ
    
    GetData::GetData(std::string const & filename) :
        Atomname([this] { return atomname_; }, nullptr),
        L([this] { return l_; }, nullptr),
        N([this] { return n_; }, nullptr),
        Orbital([this] { return orbital_; }, nullptr),
        Rhomax([this] { return rhomax_; }, nullptr),
        R_meshmin([this] { return r_meshmin_; }, nullptr),
        acc_(gsl_interp_accel_alloc(), gsl_interp_accel_deleter)
    {
        using namespace boost::algorithm;
        
        // トークン分割
        std::vector<std::string> tokens;
        split(tokens, filename, is_any_of("_"), token_compress_on);

        if (tokens[1] == "H") {
            atomname_ = "Hydrogen";
        }
        else if (tokens[1] == "He") {
            atomname_ = "Helium";
        }
        else {
            throw std::runtime_error("ファイル名が異常です！");
        }

        orbital_ = tokens[2][0];
        n_ = boost::numeric_cast<std::uint8_t>(tokens[2][0] - '0');
  
        switch (tokens[2][1]) {
        case 's':
            l_ = 0;
            orbital_ += 's';
            break;

        case 'p':
            l_ = 1;
            orbital_ += 'p';
            break;

        case 'd':
            l_ = 2;
            orbital_ += 'd';
            break;

        case 'f':
            l_ = 3;
            orbital_ += 'f';
            break;

        case 'g':
            l_ = 4;
            orbital_ += 'g';
            break;

        default:
            throw std::runtime_error("ファイル名が異常です！");
            break;
        }

        std::vector<double> r_mesh, rho;
        std::tie(r_mesh, rho) = ReadDataFile().readdatafile(filename);
        
        BOOST_ASSERT(r_mesh.size() == rho.size());

        rhomax_ = *boost::max_element(rho);
        r_meshmin_ = r_mesh[0];

        spline_ = std::unique_ptr<gsl_spline, decltype(gsl_spline_deleter)>(
            gsl_spline_alloc(gsl_interp_cspline, r_mesh.size()), gsl_spline_deleter);

        gsl_spline_init(spline_.get(), r_mesh.data(), rho.data(), r_mesh.size());
    }

    // #endregion コンストラクタ

    // #region メンバ関数

    double GetData::operator()(double r) const
    {
        return gsl_spline_eval(spline_.get(), r, acc_.get());
    }

    // #endsregion メンバ関数
}
