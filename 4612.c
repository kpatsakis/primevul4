    explicit Iterator(const typename Iterator::Params& params)
        : DatasetIterator<Dataset<T>>(params),
          num_elements_(params.dataset->sparse_tensor_.shape()[0]),
          dense_shape_(DT_INT64, {params.dataset->sparse_tensor_.dims() - 1}),
          group_iterable_(params.dataset->sparse_tensor_.group({0})),
          iter_(group_iterable_.begin()) {
      for (size_t i = 0; i < dense_shape_.NumElements(); ++i) {
        dense_shape_.vec<int64>()(i) =
            params.dataset->sparse_tensor_.shape()[i + 1];
      }
    }