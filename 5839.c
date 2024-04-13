  Status operator()(OpKernelContext* context,
                    typename TTypes<Tindex>::ConstVec reverse_index_map,
                    typename TTypes<T>::ConstVec grad_values,
                    typename TTypes<T>::Vec d_values,
                    typename TTypes<T>::Scalar d_default_value) {
    const CPUDevice& device = context->eigen_device<CPUDevice>();
    const Tindex N = reverse_index_map.dimension(0);
    const Tindex N_full = grad_values.dimension(0);

    T& d_default_value_scalar = d_default_value();
    d_default_value_scalar = T();

    Tensor visited_t;
    TF_RETURN_IF_ERROR(
        context->allocate_temp(DT_BOOL, TensorShape({N_full}), &visited_t));
    auto visited = visited_t.vec<bool>();
    visited.device(device) = visited.constant(false);

    for (int i = 0; i < N; ++i) {
      // Locate the index of the output of the forward prop associated
      // with this location in the input of the forward prop.  Copy
      // the gradient into it.  Mark it as visited.
      int64_t reverse_index = reverse_index_map(i);
      if (reverse_index < 0 || reverse_index >= N_full) {
        return errors::InvalidArgument(
            "Elements in reverse index must be in [0, ", N_full, ") but got ",
            reverse_index);
      }
      d_values(i) = grad_values(reverse_index);
      visited(reverse_index) = true;
    }
    for (int j = 0; j < N_full; ++j) {
      // The default value gradient gets the accumulated remainder of
      // the backprop values (since the default value was used to fill
      // in these slots in the forward calculation).
      if (!visited(j)) {
        d_default_value_scalar += grad_values(j);
      }
    }
    return Status::OK();
  }