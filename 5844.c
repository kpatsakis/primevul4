  Index operator()(const CPUDevice &d,
                   typename TTypes<Index>::ConstMatrix indices,
                   typename TTypes<T>::ConstFlat updates,
                   typename TTypes<T, NDIMS>::Tensor out) {
    Eigen::array<Eigen::DenseIndex, NDIMS> idx;
    const int num_nnz = static_cast<int>(indices.dimension(0));
    for (int i = 0; i < num_nnz; ++i) {
      for (int d = 0; d < NDIMS; ++d) {
        idx[d] = internal::SubtleMustCopy(indices(i, d));
        if (!FastBoundsCheck(idx[d], out.dimension(d))) {
          return d;  // on failure: d nonnegative
        }
      }
      out(idx) += updates(i);
    }
    return -1;  // on success
  }