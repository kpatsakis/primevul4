  void Compute(OpKernelContext* const context) override {
    ResourceHandle handle;
    OP_REQUIRES_OK(context,
                   HandleFromInput(context, kResourceHandleName, &handle));
    core::RefCountPtr<QuantileStreamResource> stream_resource;
    OP_REQUIRES_OK(context, LookupResource(context, handle, &stream_resource));
    // Remove the reference at the end of this scope.
    mutex_lock l(*stream_resource->mutex());

    OpOutputList summaries_output_list;
    OP_REQUIRES_OK(
        context, context->output_list(kSummariesName, &summaries_output_list));

    auto do_quantile_summary_gen = [&](const int64_t begin, const int64_t end) {
      // Iterating features.
      for (int64_t index = begin; index < end; index++) {
        QuantileStream* stream = stream_resource->stream(index);
        stream->Finalize();

        const auto summary_list = stream->GetFinalSummary().GetEntryList();
        Tensor* output_t;
        const int64_t summary_list_size =
            static_cast<int64>(summary_list.size());
        OP_REQUIRES_OK(context, summaries_output_list.allocate(
                                    index, TensorShape({summary_list_size, 4}),
                                    &output_t));
        auto output = output_t->matrix<float>();
        for (auto row = 0; row < summary_list_size; row++) {
          const auto& entry = summary_list[row];
          output(row, 0) = entry.value;
          output(row, 1) = entry.weight;
          output(row, 2) = entry.min_rank;
          output(row, 3) = entry.max_rank;
        }
      }
    };
    // TODO(tanzheny): comment on the magic number.
    const int64_t kCostPerUnit = 500 * num_features_;
    const DeviceBase::CpuWorkerThreads& worker_threads =
        *context->device()->tensorflow_cpu_worker_threads();
    Shard(worker_threads.num_threads, worker_threads.workers, num_features_,
          kCostPerUnit, do_quantile_summary_gen);
    stream_resource->ResetStreams();
  }