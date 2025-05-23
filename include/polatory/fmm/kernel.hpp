#pragma once

#include <Eigen/Core>
#include <array>
#include <polatory/geometry/point3d.hpp>
#include <polatory/rbf/rbf_base.hpp>
#include <scalfmm/container/point.hpp>
#include <scalfmm/matrix_kernels/mk_common.hpp>
#include <string>
#include <type_traits>

namespace polatory::fmm {

template <class Rbf_>
struct Kernel {
  using Rbf = Rbf_;
  static constexpr int kDim = Rbf::kDim;

  template <class OtherRbf>
  using Rebind = Kernel<OtherRbf>;

  using Vector = geometry::Vector<kDim>;

  static constexpr auto homogeneity_tag{scalfmm::matrix_kernels::homogeneity::non_homogenous};
  static constexpr auto symmetry_tag{scalfmm::matrix_kernels::symmetry::symmetric};
  static constexpr std::size_t km{1};
  static constexpr std::size_t kn{1};
  template <typename ValueType>
  using matrix_type = std::array<ValueType, kn * km>;
  template <typename ValueType>
  using vector_type = std::array<ValueType, kn>;

  explicit Kernel(const Rbf& rbf) : rbf_(rbf) {}

  std::string name() const { return ""; }

  template <typename ValueType>
  constexpr auto mutual_coefficient() const {
    using decayed_type = typename std::decay_t<ValueType>;
    return vector_type<decayed_type>{decayed_type(1.0)};
  }

  template <class T, class U>
  auto evaluate(scalfmm::container::point<T, kDim> const& x,
                scalfmm::container::point<U, kDim> const& y) const {
    Vector diff;
    for (auto i = 0; i < kDim; i++) {
      diff(i) = x.at(i) - y.at(i);
    }

    return matrix_type<double>{rbf_.evaluate_isotropic(diff)};
  }

  auto evaluate(scalfmm::container::point<xsimd::batch<double>, kDim> const& x,
                scalfmm::container::point<xsimd::batch<double>, kDim> const& y) const {
    using decayed_type = typename std::decay_t<xsimd::batch<double>>;

    std::array<double, 4> v{};
    for (std::size_t i = 0; i < x.at(0).size; i++) {
      Vector diff;
      for (auto j = 0; j < kDim; j++) {
        diff(j) = x.at(j).get(i) - y.at(j).get(i);
      }

      v.at(i) = rbf_.evaluate_isotropic(diff);
    }

    return matrix_type<decayed_type>{decayed_type::load(v.data(), xsimd::unaligned_mode{})};
  }

  static constexpr int separation_criterion{1};

 private:
  const Rbf rbf_;
};

}  // namespace polatory::fmm
