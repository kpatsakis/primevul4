  void Compute(OpKernelContext* const context) override {
    // Read float features list;
    OpInputList float_features_list;
    OP_REQUIRES_OK(
        context, context->input_list(kFloatFeaturesName, &float_features_list));
    OpInputList bucket_boundaries_list;
    OP_REQUIRES_OK(context, context->input_list(kBucketBoundariesName,
                                                &bucket_boundaries_list));
    OP_REQUIRES(context,
                tensorflow::TensorShapeUtils::IsVector(
                    bucket_boundaries_list[0].shape()),
                errors::InvalidArgument(
                    strings::Printf("Buckets should be flat vectors.")));
    OpOutputList buckets_list;
    OP_REQUIRES_OK(context, context->output_list(kBucketsName, &buckets_list));

    auto do_quantile_get_quantiles = [&](const int64_t begin,
                                         const int64_t end) {
      // Iterating over all resources
      for (int64_t feature_idx = begin; feature_idx < end; feature_idx++) {
        const Tensor& values_tensor = float_features_list[feature_idx];
        const int64_t num_values = values_tensor.dim_size(0);

        Tensor* output_t = nullptr;
        OP_REQUIRES_OK(context,
                       buckets_list.allocate(
                           feature_idx, TensorShape({num_values}), &output_t));
        auto output = output_t->flat<int32>();

        const std::vector<float>& bucket_boundaries_vector =
            GetBuckets(feature_idx, bucket_boundaries_list);
        auto flat_values = values_tensor.flat<float>();
        const auto& iter_begin = bucket_boundaries_vector.begin();
        const auto& iter_end = bucket_boundaries_vector.end();
        for (int64_t instance = 0; instance < num_values; instance++) {
          if (iter_begin == iter_end) {
            output(instance) = 0;
            continue;
          }
          const float value = flat_values(instance);
          auto bucket_iter = std::lower_bound(iter_begin, iter_end, value);
          if (bucket_iter == iter_end) {
            --bucket_iter;
          }
          const int32_t bucket = static_cast<int32>(bucket_iter - iter_begin);
          // Bucket id.
          output(instance) = bucket;
        }
      }
    };

    // TODO(tanzheny): comment on the magic number.
    const int64_t kCostPerUnit = 500 * num_features_;
    const DeviceBase::CpuWorkerThreads& worker_threads =
        *context->device()->tensorflow_cpu_worker_threads();
    Shard(worker_threads.num_threads, worker_threads.workers, num_features_,
          kCostPerUnit, do_quantile_get_quantiles);
  }