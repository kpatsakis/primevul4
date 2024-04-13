    Status GetNextInternal(IteratorContext* ctx,
                           std::vector<Tensor>* out_tensors,
                           bool* end_of_sequence) override {
      mutex_lock l(mu_);
      if (i_ == num_elements_) {
        *end_of_sequence = true;
        return Status::OK();
      }

      out_tensors->clear();
      out_tensors->reserve(3);
      const int rank = Iterator::dataset()->sparse_tensor_.dims();

      if (i_ > next_non_empty_i_ && iter_ != group_iterable_.end()) {
        // We still have elements to consume from `group_iterable_`
        // and we have emitted all elements up to and including the
        // current position.
        sparse::Group group = *iter_;
        const auto indices = group.indices();
        const auto values = group.values<T>();
        const int64_t num_entries = values.size();
        next_non_empty_i_ = indices(0, 0);

        next_indices_ = Tensor(DT_INT64, {num_entries, rank - 1});
        next_values_ = Tensor(DataTypeToEnum<T>::value, {num_entries});

        auto next_indices_t = next_indices_.matrix<int64>();
        auto next_values_t = next_values_.vec<T>();

        for (int64_t i = 0; i < num_entries; ++i) {
          for (int d = 1; d < rank; ++d) {
            next_indices_t(i, d - 1) = indices(i, d);
          }
          next_values_t(i) = values(i);
        }

        ++iter_;
      }
      if (i_ == next_non_empty_i_) {
        // The current position is non-empty in the input
        // `SparseTensor`, and we have already read the value from the
        // `GroupIterable`.
        out_tensors->push_back(std::move(next_indices_));
        out_tensors->push_back(std::move(next_values_));
        out_tensors->push_back(dense_shape_);
        next_non_empty_i_ = kNextNonEmptyUnknown;
      } else {
        DCHECK(i_ < next_non_empty_i_ || iter_ == group_iterable_.end());
        // The current position is empty in the input `SparseTensor`,
        // so emit empty indices and values.
        out_tensors->push_back(Tensor(DT_INT64, TensorShape({0, rank - 1})));
        out_tensors->push_back(Tensor(DataTypeToEnum<T>::value, {0}));
        out_tensors->push_back(dense_shape_);
      }

      ++i_;
      *end_of_sequence = false;
      return Status::OK();
    }