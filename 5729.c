  void ComputeMatrix(OpKernelContext* context, const ConstMatrixMaps& inputs,
                     MatrixMaps* outputs) final {
    const ConstMatrixMap& matrix = inputs[0];
    const ConstMatrixMap& rhs = inputs[1];
    if (matrix.rows() == 0 || matrix.cols() == 0 || rhs.cols() == 0) {
      // To be consistent with the MatrixInverse op, we define the solution for
      // an empty set of equation as the empty matrix.
      return;
    }
    Eigen::PartialPivLU<Matrix> lu_decomposition(matrix.rows());
    if (adjoint_) {
      // TODO(rmlarsen): For Eigen 3.2, this creates a temporary copy.
      // Make sure to backport: https://bitbucket.org/eigen/eigen/commits/
      // bd2219a74c96dfe3f6bc2c23588749e36d2d8173
      lu_decomposition.compute(matrix.adjoint());
    } else {
      lu_decomposition.compute(matrix);
    }

    // PartialPivLU cannot give strong guarantees on invertibility,
    // but we can at least guard against exact zero pivots. This can occur as
    // a result of basic user mistakes such providing integer valued
    // matrices that are exactly singular, or due to underflow if this
    // code is run with denormals being flushed to zero.
    const RealScalar min_abs_pivot =
        lu_decomposition.matrixLU().diagonal().cwiseAbs().minCoeff();
    OP_REQUIRES(context, min_abs_pivot > RealScalar(0),
                errors::InvalidArgument(kErrMsg));

    // TODO(rmlarsen): Add check based on condition number estimation.
    // The necessary changes to Eigen are in
    // https://bitbucket.org/eigen/eigen/pull-requests/174/
    // add-matrix-condition-number-estimation/diff
    outputs->at(0) = lu_decomposition.solve(rhs);
  }