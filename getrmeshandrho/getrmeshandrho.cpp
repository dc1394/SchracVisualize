/*! \file getrmeshandrho.h
    \brief rのメッシュと、そのメッシュにおける電子密度を与えるクラスの実装

    Copyright © 2015 @dc1394 All Rights Reserved.
*/

#include "DXUT.h"
#include "getrmeshandrho.h"
#include "readdatafile.h"
#include <tuple>                        // for std::tie
#include <boost/assert.hpp>             // for BOOST_ASSERT
#include <boost/range/algorithm.hpp>    // for boost::max_element

namespace getrmeshandrho {
    // #region コンストラクタ
    
    GetRMeshAndRho::GetRMeshAndRho(std::string const & filename) :
        Rhomax([this] { return rhomax_; }, nullptr),
        R_meshmin([this] { return r_meshmin_; }, nullptr),
        acc_(gsl_interp_accel_alloc(), gsl_interp_accel_deleter)
    {
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

    double GetRMeshAndRho::operator()(double r) const
    {
        return gsl_spline_eval(spline_.get(), r, acc_.get());
    }

    // #endsregion メンバ関数
}
