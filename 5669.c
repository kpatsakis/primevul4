  void Compute(OpKernelContext* c) override {
    core::RefCountPtr<Var> v;
    OP_REQUIRES_OK(c, LookupResource(c, HandleFromInput(c, 0), &v));
    OP_REQUIRES_OK(c, EnsureSparseVariableAccess<Device, T>(c, v.get()));
    // NOTE: We hold the lock for the whole gather operation instead
    // of increasing the reference count of v->tensor() to avoid a
    // situation where a write to the same variable will see a
    // reference count greater than one and make a copy of the
    // (potentially very large) tensor buffer.
    tf_shared_lock ml(*v->mu());
    const Tensor& params = *v->tensor();
    const Tensor& indices = c->input(1);
    OP_REQUIRES(
        c, TensorShapeUtils::IsVectorOrHigher(params.shape()),
        errors::InvalidArgument("params must be at least 1 dimensional"));
    OP_REQUIRES(
        c, params.shape().dims() >= batch_dims_,
        errors::InvalidArgument("params must have at least ", batch_dims_,
                                " (batch_dims) dimensions but it has shape ",
                                params.shape().DebugString()));

    // Check that we have enough index space
    const int64_t N = indices.NumElements();
    OP_REQUIRES(
        c, params.dim_size(0) <= std::numeric_limits<Index>::max(),
        errors::InvalidArgument("params.shape[0] too large for ",
                                DataTypeString(DataTypeToEnum<Index>::v()),
                                " indexing: ", params.dim_size(0), " > ",
                                std::numeric_limits<Index>::max()));

    // The result shape is params.shape[:batch_dims] +
    // indices.shape[batch_dims:] + params.shape[batch_dims+1:].
    TensorShape result_shape;
    for (int i = 0; i < batch_dims_; ++i) {
      result_shape.AddDim(params.dim_size(i));
    }
    for (int i = batch_dims_; i < indices.dims(); ++i) {
      result_shape.AddDim(indices.dim_size(i));
    }
    for (int i = batch_dims_ + 1; i < params.dims(); ++i) {
      result_shape.AddDim(params.dim_size(i));
    }

    Tensor* out = nullptr;
    Tensor tmp;
    if (params.dtype() == DT_VARIANT) {
      tmp = Tensor(DT_VARIANT, result_shape);
      c->set_output(0, tmp);
      out = &tmp;
    } else {
      OP_REQUIRES_OK(c, c->allocate_output(0, result_shape, &out));
    }

    if (N > 0) {
      Tensor tmp_indices;

      // Points to the original or updated (if batch_dims is set) indices.
      const Tensor* op_indices = &indices;
      if (batch_dims_ > 0) {
        OP_REQUIRES_OK(c, c->allocate_temp(indices.dtype(), indices.shape(),
                                           &tmp_indices));
        functor::DenseUpdate<Device, Index, ASSIGN> copy_functor;
        copy_functor(c->eigen_device<Device>(), tmp_indices.flat<Index>(),
                     indices.flat<Index>());

        AddBatchOffsets(c, &tmp_indices, params);
        if (!c->status().ok()) return;
        op_indices = &tmp_indices;
      }

      int64_t gather_dim_size = 1;
      for (int idx = 0; idx <= batch_dims_; ++idx) {
        gather_dim_size *= params.dim_size(idx);
      }
      int64_t inner_size = 1;
      for (int i = batch_dims_ + 1; i < params.dims(); ++i) {
        inner_size *= params.dim_size(i);
      }
      auto params_flat = params.shaped<T, 3>({1, gather_dim_size, inner_size});
      const auto indices_flat = op_indices->flat<Index>();
      auto out_flat = out->shaped<T, 3>({1, N, out->NumElements() / N});

      functor::GatherFunctor<Device, T, Index> functor;
      int64_t bad_i = functor(c, params_flat, indices_flat, out_flat);

      OP_REQUIRES(
          c, bad_i < 0,
          errors::InvalidArgument(
              "indices", SliceDebugString(indices.shape(), bad_i), " = ",
              indices_flat(bad_i), " is not in [0, ", params.dim_size(0), ")"));
    }
  }