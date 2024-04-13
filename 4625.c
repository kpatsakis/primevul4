  void Compute(OpKernelContext* const context) override {
    ResourceHandle handle;
    OP_REQUIRES_OK(context,
                   HandleFromInput(context, kResourceHandleName, &handle));
    core::RefCountPtr<QuantileStreamResource> stream_resource;
    // Create a reference to the underlying resource using the handle.
    OP_REQUIRES_OK(context, LookupResource(context, handle, &stream_resource));
    // Remove the reference at the end of this scope.
    mutex_lock l(*stream_resource->mutex());

    const int64_t num_streams = stream_resource->num_streams();
    CHECK_EQ(num_features_, num_streams);
    OpOutputList bucket_boundaries_list;
    OP_REQUIRES_OK(context, context->output_list(kBucketBoundariesName,
                                                 &bucket_boundaries_list));

    auto do_quantile_get_buckets = [&](const int64_t begin, const int64_t end) {
      // Iterating over all streams.
      for (int64_t stream_idx = begin; stream_idx < end; stream_idx++) {
        const auto& boundaries = stream_resource->boundaries(stream_idx);
        Tensor* bucket_boundaries_t = nullptr;
        OP_REQUIRES_OK(context,
                       bucket_boundaries_list.allocate(
                           stream_idx, {static_cast<int64>(boundaries.size())},
                           &bucket_boundaries_t));
        auto* quantiles_flat = bucket_boundaries_t->flat<float>().data();
        memcpy(quantiles_flat, boundaries.data(),
               sizeof(float) * boundaries.size());
      }
    };

    // TODO(tanzheny): comment on the magic number.
    const int64_t kCostPerUnit = 500 * num_streams;
    const DeviceBase::CpuWorkerThreads& worker_threads =
        *context->device()->tensorflow_cpu_worker_threads();
    Shard(worker_threads.num_threads, worker_threads.workers, num_streams,
          kCostPerUnit, do_quantile_get_buckets);
  }