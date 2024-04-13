  void Compute(OpKernelContext *ctx) override {
    const Tensor *a_indices_t, *a_values_t, *a_shape_t, *b;
    OP_REQUIRES_OK(ctx, ctx->input("a_indices", &a_indices_t));
    OP_REQUIRES_OK(ctx, ctx->input("a_values", &a_values_t));
    OP_REQUIRES_OK(ctx, ctx->input("a_shape", &a_shape_t));
    OP_REQUIRES_OK(ctx, ctx->input("b", &b));
    OP_REQUIRES_OK(
        ctx, ValidateInputs<Index>(a_indices_t, a_values_t, a_shape_t, b));

    Tensor *out_t;
    OP_REQUIRES_OK(ctx, ctx->allocate_output(0, b->shape(), &out_t));

    const int ndims = static_cast<int>(a_indices_t->dim_size(1));
    const auto a_indices_mat = a_indices_t->flat_inner_dims<Index>();
    const auto a_values_flat = a_values_t->flat<T>();

    switch (ndims) {
#define NDIMS_CASE(N)                                                     \
  case N: {                                                               \
    auto out_tensor = out_t->tensor<T, N>();                              \
    out_tensor.device(ctx->eigen_device<Device>()) = b->tensor<T, N>();   \
    const Index result =                                                  \
        functor::ScatterNdFunctor<Device, T, Index, N,                    \
                                  scatter_op::UpdateOp::ADD>()(           \
            ctx->eigen_device<Device>(), a_indices_mat, a_values_flat,    \
            out_tensor);                                                  \
    OP_REQUIRES(                                                          \
        ctx, result == -1,                                                \
        errors::InvalidArgument(                                          \
            "Sparse tensor has some invalid index on dimension ", result, \
            "; dense tensor shape: ", b->shape().DebugString()));         \
  } break;

      NDIMS_CASE(1);
      NDIMS_CASE(2);
      NDIMS_CASE(3);
      NDIMS_CASE(4);
      NDIMS_CASE(5);
      default:
        OP_REQUIRES(
            ctx, false,
            errors::InvalidArgument("Only tensors with ranks between 1 and 5 "
                                    "are currently supported.  Tensor rank: ",
                                    ndims));
#undef NDIMS_CASE
    }
  }