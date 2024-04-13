  void Compute(OpKernelContext* context) override {
    ResourceHandle handle;
    OP_REQUIRES_OK(context,
                   HandleFromInput(context, kResourceHandleName, &handle));
    core::RefCountPtr<QuantileStreamResource> stream_resource;
    // Create a reference to the underlying resource using the handle.
    OP_REQUIRES_OK(context, LookupResource(context, handle, &stream_resource));
    // Remove the reference at the end of this scope.
    mutex_lock l(*stream_resource->mutex());

    OpInputList summaries_list;
    OP_REQUIRES_OK(context,
                   context->input_list(kSummariesName, &summaries_list));
    int32_t num_streams = stream_resource->num_streams();
    CHECK_EQ(static_cast<int>(num_streams), summaries_list.size());

    auto do_quantile_add_summary = [&](const int64_t begin, const int64_t end) {
      // Iterating all features.
      for (int64_t feature_idx = begin; feature_idx < end; ++feature_idx) {
        QuantileStream* stream = stream_resource->stream(feature_idx);
        if (stream->IsFinalized()) {
          VLOG(1) << "QuantileStream has already been finalized for feature"
                  << feature_idx << ".";
          continue;
        }
        const Tensor& summaries = summaries_list[feature_idx];
        const auto summary_values = summaries.matrix<float>();
        const auto& tensor_shape = summaries.shape();
        const int64_t entries_size = tensor_shape.dim_size(0);
        CHECK_EQ(tensor_shape.dim_size(1), 4);
        std::vector<QuantileSummaryEntry> summary_entries;
        summary_entries.reserve(entries_size);
        for (int64_t i = 0; i < entries_size; i++) {
          float value = summary_values(i, 0);
          float weight = summary_values(i, 1);
          float min_rank = summary_values(i, 2);
          float max_rank = summary_values(i, 3);
          QuantileSummaryEntry entry(value, weight, min_rank, max_rank);
          summary_entries.push_back(entry);
        }
        stream_resource->stream(feature_idx)->PushSummary(summary_entries);
      }
    };

    // TODO(tanzheny): comment on the magic number.
    const int64_t kCostPerUnit = 500 * num_streams;
    const DeviceBase::CpuWorkerThreads& worker_threads =
        *context->device()->tensorflow_cpu_worker_threads();
    Shard(worker_threads.num_threads, worker_threads.workers, num_streams,
          kCostPerUnit, do_quantile_add_summary);
  }