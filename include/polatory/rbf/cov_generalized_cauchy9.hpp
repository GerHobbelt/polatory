#pragma once

#include <cmath>
#include <polatory/rbf/covariance_function_base.hpp>
#include <polatory/rbf/rbf_proxy.hpp>
#include <vector>

namespace polatory::rbf {

namespace internal {

template <int Dim>
class cov_generalized_cauchy9 final : public covariance_function_base<Dim> {
 public:
  static constexpr int kDim = Dim;
  static inline const std::string kShortName = "gc9";

 private:
  using Base = covariance_function_base<Dim>;
  using Matrix = Base::Matrix;
  using RbfPtr = Base::RbfPtr;
  using Vector = Base::Vector;

 public:
  using Base::Base;
  using Base::parameters;
  using Base::set_parameters;

  explicit cov_generalized_cauchy9(const std::vector<double>& params) { set_parameters(params); }

  RbfPtr clone() const override { return std::make_unique<cov_generalized_cauchy9>(*this); }

  double evaluate_isotropic(const Vector& diff) const override {
    auto psill = parameters().at(0);
    auto range = parameters().at(1);
    auto r = diff.norm();
    auto rho = r / range;

    return psill * std::pow(1.0 + rho * rho, -4.5);
  }

  Vector evaluate_gradient_isotropic(const Vector& diff) const override {
    auto psill = parameters().at(0);
    auto range = parameters().at(1);
    auto r = diff.norm();
    auto rho = r / range;

    auto coeff = -9.0 * psill * std::pow(1.0 + rho * rho, -5.5) / (range * range);
    return coeff * diff;
  }

  Matrix evaluate_hessian_isotropic(const Vector& diff) const override {
    auto psill = parameters().at(0);
    auto range = parameters().at(1);
    auto r = diff.norm();
    auto rho = r / range;

    auto coeff = -9.0 * psill * std::pow(1.0 + rho * rho, -5.5) / (range * range);
    return coeff * (Matrix::Identity() - 11.0 / (r * r + range * range) * diff.transpose() * diff);
  }

  std::string short_name() const override { return kShortName; }
};

}  // namespace internal

POLATORY_DEFINE_RBF(cov_generalized_cauchy9);

}  // namespace polatory::rbf
