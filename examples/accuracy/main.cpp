// This is an example program that estimates the relative accuracy of
// the fast evaluation method of RBF interpolants.

#include <Eigen/Core>
#include <exception>
#include <iostream>
#include <polatory/interpolation/rbf_direct_evaluator.hpp>
#include <polatory/polatory.hpp>

#include "../common/common.hpp"
#include "parse_options.hpp"

using polatory::index_t;
using polatory::model;
using polatory::common::valuesd;
using polatory::geometry::bboxNd;
using polatory::geometry::pointNd;
using polatory::geometry::pointsNd;
using polatory::interpolation::rbf_direct_evaluator;
using polatory::interpolation::rbf_evaluator;

template <class Rbf>
void main_impl(Rbf&& rbf, const options& opts) {
  constexpr int kDim = Rbf::kDim;
  using Bbox = bboxNd<kDim>;
  using Point = pointNd<kDim>;
  using Points = pointsNd<kDim>;

  auto poly_degree = rbf.cpd_order() - 1;
  model model(rbf, poly_degree);

  auto mu = opts.n_points;
  auto sigma = opts.n_grad_points;
  auto m = mu + kDim * sigma;
  auto l = model.poly_basis_size();

  Points points = Points::Random(mu, kDim);
  Points grad_points = Points::Random(sigma, kDim);
  Points eval_points = Points::Random(opts.n_eval_points, kDim);
  Points grad_eval_points = Points::Random(opts.n_grad_eval_points, kDim);

  valuesd weights = valuesd::Zero(m + l);
  weights.head(m) = valuesd::Random(m);

  rbf_direct_evaluator direct_eval(model, points, grad_points);
  direct_eval.set_weights(weights);
  direct_eval.set_target_points(eval_points, grad_eval_points);

  Bbox bbox{-Point::Ones(), Point::Ones()};
  rbf_evaluator eval(model, bbox, opts.order);
  eval.set_source_points(points, grad_points);
  eval.set_weights(weights);
  eval.set_target_points(eval_points, grad_eval_points);

  auto direct_values = direct_eval.evaluate();
  auto values = eval.evaluate();

  std::cout << "Relative error (L1): "
            << (values - direct_values).template lpNorm<1>() / direct_values.template lpNorm<1>()
            << std::endl;
  std::cout << "Relative error (L2): " << (values - direct_values).norm() / direct_values.norm()
            << std::endl;
  std::cout << "Relative error (L-infinity): "
            << (values - direct_values).template lpNorm<Eigen::Infinity>() /
                   direct_values.template lpNorm<Eigen::Infinity>()
            << std::endl;
}

int main(int argc, const char* argv[]) {
  try {
    auto opts = parse_options(argc, argv);
    MAIN_IMPL(opts.rbf_name, opts.dim, opts.rbf_params, opts);
    return 0;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
