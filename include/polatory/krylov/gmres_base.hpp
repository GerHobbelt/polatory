#pragma once

#include <polatory/krylov/linear_operator.hpp>
#include <polatory/types.hpp>
#include <vector>

namespace polatory::krylov {

class gmres_base {
 public:
  virtual ~gmres_base() = default;

  gmres_base(const gmres_base&) = delete;
  gmres_base(gmres_base&&) = delete;
  gmres_base& operator=(const gmres_base&) = delete;
  gmres_base& operator=(gmres_base&&) = delete;

  double absolute_residual() const;

  bool converged() const;

  virtual void iterate_process() = 0;

  index_t iteration_count() const;

  index_t max_iterations() const;

  double relative_residual() const;

  virtual void set_left_preconditioner(const linear_operator& left_preconditioner);

  void set_initial_solution(const vectord& x0);

  virtual void set_right_preconditioner(const linear_operator& right_preconditioner);

  virtual void setup();

  virtual vectord solution_vector() const;

 protected:
  gmres_base(const linear_operator& op, const vectord& rhs, index_t max_iter);

  virtual void add_preconditioned_krylov_basis(const vectord& /*z*/) {}

  vectord left_preconditioned(const vectord& x) const;

  vectord right_preconditioned(const vectord& x) const;

  const linear_operator& op_;

  // Dimension.
  const index_t m_;

  // Maximum # of iteration.
  const index_t max_iter_;

  // Initial solution.
  vectord x0_;

  // Left preconditioner.
  const linear_operator* left_pc_{};

  // Right preconditioner.
  const linear_operator* right_pc_{};

  // Current # of iteration.
  index_t iter_{};

  // Constant (right-hand side) vector.
  const vectord rhs_;

  // L2 norm of rhs.
  double rhs_norm_;

  // Orthonormal basis vectors for the Krylov subspace.
  std::vector<vectord> vs_;

  // Upper triangular matrix of QR decomposition.
  matrixd r_;

  // Cosines for the Givens rotations.
  vectord c_;

  // Sines for the Givens rotations.
  vectord s_;

  // Sequence of residuals.
  vectord g_;

  bool converged_{};
};

}  // namespace polatory::krylov
