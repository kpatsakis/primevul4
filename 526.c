  void Compute(OpKernelContext* context) override {
    const Tensor* input_indices;
    const Tensor* input_values;
    const Tensor* input_shape;
    SparseTensorsMap* map;

    OP_REQUIRES_OK(context, context->input("sparse_indices", &input_indices));
    OP_REQUIRES_OK(context, context->input("sparse_values", &input_values));
    OP_REQUIRES_OK(context, context->input("sparse_shape", &input_shape));
    OP_REQUIRES_OK(context, GetMap(context, true /* is_writing */, &map));

    OP_REQUIRES(context, TensorShapeUtils::IsMatrix(input_indices->shape()),
                errors::InvalidArgument(
                    "Input indices should be a matrix but received shape ",
                    input_indices->shape().DebugString()));
    OP_REQUIRES(context, TensorShapeUtils::IsVector(input_values->shape()),
                errors::InvalidArgument(
                    "Input values should be a vector but received shape ",
                    input_values->shape().DebugString()));
    OP_REQUIRES(context, TensorShapeUtils::IsVector(input_shape->shape()),
                errors::InvalidArgument(
                    "Input shape should be a vector but received shape ",
                    input_shape->shape().DebugString()));
    OP_REQUIRES(
        context,
        input_values->shape().dim_size(0) == input_indices->shape().dim_size(0),
        errors::InvalidArgument(
            "Number of values must match first dimension of indices. ", "Got ",
            input_values->shape().dim_size(0),
            " values, indices shape: ", input_indices->shape().DebugString()));
    OP_REQUIRES(
        context,
        input_shape->shape().dim_size(0) == input_indices->shape().dim_size(1),
        errors::InvalidArgument(
            "Number of dimensions must match second dimension of indices. ",
            "Got ", input_shape->shape().dim_size(0),
            " dimensions, indices shape: ",
            input_indices->shape().DebugString()));

    int rank = input_shape->NumElements();

    OP_REQUIRES(
        context, rank > 1,
        errors::InvalidArgument(
            "Rank of input SparseTensor should be > 1, but saw rank: ", rank));

    auto input_shape_vec = input_shape->vec<int64_t>();
    int new_num_elements = 1;
    bool overflow_ocurred = false;
    for (int i = 0; i < input_shape_vec.size(); i++) {
      new_num_elements =
          MultiplyWithoutOverflow(new_num_elements, input_shape_vec(i));
      if (new_num_elements < 0) {
        overflow_ocurred = true;
        break;
      }
    }

    OP_REQUIRES(
        context, !overflow_ocurred,
        errors::Internal("Encountered overflow from large input shape."));

    TensorShape tensor_input_shape(input_shape_vec);
    gtl::InlinedVector<int64_t, 8> std_order(rank);
    std::iota(std_order.begin(), std_order.end(), 0);
    SparseTensor input_st;
    OP_REQUIRES_OK(context, SparseTensor::Create(*input_indices, *input_values,
                                                 tensor_input_shape, std_order,
                                                 &input_st));

    const int64_t N = input_shape_vec(0);

    Tensor sparse_handles(DT_INT64, TensorShape({N}));
    auto sparse_handles_t = sparse_handles.vec<int64_t>();

    OP_REQUIRES_OK(context, input_st.IndicesValid());

    // We can generate the output shape proto string now, for all
    // minibatch entries.
    TensorShape output_shape;
    OP_REQUIRES_OK(context, TensorShapeUtils::MakeShape(
                                input_shape_vec.data() + 1,
                                input_shape->NumElements() - 1, &output_shape));

    // Get groups by minibatch dimension
    std::unordered_set<int64_t> visited;
    sparse::GroupIterable minibatch = input_st.group({0});
    for (const auto& subset : minibatch) {
      const int64_t b = subset.group()[0];
      visited.insert(b);
      OP_REQUIRES(
          context, b > -1 && b < N,
          errors::InvalidArgument(
              "Received unexpected column 0 value in input SparseTensor: ", b,
              " < 0 or >= N (= ", N, ")"));

      const auto indices = subset.indices();
      const auto values = subset.values<T>();
      const int64_t num_entries = values.size();

      Tensor output_indices = Tensor(DT_INT64, {num_entries, rank - 1});
      Tensor output_values = Tensor(DataTypeToEnum<T>::value, {num_entries});

      auto output_indices_t = output_indices.matrix<int64_t>();
      auto output_values_t = output_values.vec<T>();

      for (int i = 0; i < num_entries; ++i) {
        for (int d = 1; d < rank; ++d) {
          output_indices_t(i, d - 1) = indices(i, d);
        }
        output_values_t(i) = values(i);
      }

      SparseTensor st_i;
      OP_REQUIRES_OK(context,
                     SparseTensor::Create(output_indices, output_values,
                                          output_shape, &st_i));
      int64_t handle;
      OP_REQUIRES_OK(context, map->AddSparseTensor(context, st_i, &handle));
      sparse_handles_t(b) = handle;
    }

    // Fill in any gaps; we must provide an empty ST for batch entries
    // the grouper didn't find.
    if (visited.size() < N) {
      Tensor empty_indices(DT_INT64, {0, rank - 1});
      Tensor empty_values(DataTypeToEnum<T>::value, {0});
      SparseTensor empty_st;
      OP_REQUIRES_OK(context, SparseTensor::Create(empty_indices, empty_values,
                                                   output_shape, &empty_st));

      for (int64_t b = 0; b < N; ++b) {
        // We skipped this batch entry.
        if (visited.find(b) == visited.end()) {
          int64_t handle;
          OP_REQUIRES_OK(context,
                         map->AddSparseTensor(context, empty_st, &handle));
          sparse_handles_t(b) = handle;
        }
      }
    }

    context->set_output(0, sparse_handles);
  }