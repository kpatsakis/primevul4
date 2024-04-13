  void Compute(OpKernelContext* context) override {
    ResourceHandle handle;
    OP_REQUIRES_OK(context,
                   HandleFromInput(context, kResourceHandleName, &handle));
    core::RefCountPtr<QuantileStreamResource> stream_resource;
    // Create a reference to the underlying resource using the handle.
    OP_REQUIRES_OK(context, LookupResource(context, handle, &stream_resource));
    // Remove the reference at the end of this scope.
    mutex_lock l(*stream_resource->mutex());

    const Tensor* num_buckets_t;
    OP_REQUIRES_OK(context, context->input(kNumBucketsName, &num_buckets_t));
    const int64_t num_buckets = num_buckets_t->scalar<int64>()();
    const int64_t num_streams = stream_resource->num_streams();

    auto do_quantile_flush = [&](const int64_t begin, const int64_t end) {
      // Iterating over all streams.
      for (int64_t stream_idx = begin; stream_idx < end; ++stream_idx) {
        QuantileStream* stream = stream_resource->stream(stream_idx);
        stream->Finalize();
        stream_resource->set_boundaries(
            generate_quantiles_ ? GenerateQuantiles(*stream, num_buckets)
                                : GenerateBoundaries(*stream, num_buckets),
            stream_idx);
      }
    };

    // TODO(tanzheny): comment on the magic number.
    const int64_t kCostPerUnit = 500 * num_streams;
    const DeviceBase::CpuWorkerThreads& worker_threads =
        *context->device()->tensorflow_cpu_worker_threads();
    Shard(worker_threads.num_threads, worker_threads.workers, num_streams,
          kCostPerUnit, do_quantile_flush);

    stream_resource->ResetStreams();
    stream_resource->set_buckets_ready(true);
  }