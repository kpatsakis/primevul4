  void Compute(OpKernelContext* const context) override {
    // Read float features list;
    OpInputList float_features_list;
    OP_REQUIRES_OK(
        context, context->input_list(kFloatFeaturesName, &float_features_list));

    // Parse example weights and get batch size.
    const Tensor* example_weights_t;
    OP_REQUIRES_OK(context,
                   context->input(kExampleWeightsName, &example_weights_t));
    DCHECK(float_features_list.size() > 0) << "Got empty feature list";
    auto example_weights = example_weights_t->flat<float>();
    const int64_t weight_size = example_weights.size();
    const int64_t batch_size = float_features_list[0].flat<float>().size();
    OP_REQUIRES(
        context, weight_size == 1 || weight_size == batch_size,
        errors::InvalidArgument(strings::Printf(
            "Weights should be a single value or same size as features.")));
    const Tensor* epsilon_t;
    OP_REQUIRES_OK(context, context->input(kEpsilonName, &epsilon_t));
    float epsilon = epsilon_t->scalar<float>()();

    OpOutputList summaries_output_list;
    OP_REQUIRES_OK(
        context, context->output_list(kSummariesName, &summaries_output_list));

    auto do_quantile_summary_gen = [&](const int64_t begin, const int64_t end) {
      // Iterating features.
      for (int64_t index = begin; index < end; index++) {
        const auto feature_values = float_features_list[index].flat<float>();
        QuantileStream stream(epsilon, batch_size + 1);
        // Run quantile summary generation.
        for (int64_t j = 0; j < batch_size; j++) {
          stream.PushEntry(feature_values(j), (weight_size > 1)
                                                  ? example_weights(j)
                                                  : example_weights(0));
        }
        stream.Finalize();
        const auto summary_entry_list = stream.GetFinalSummary().GetEntryList();
        Tensor* output_t;
        OP_REQUIRES_OK(
            context,
            summaries_output_list.allocate(
                index,
                TensorShape({static_cast<int64>(summary_entry_list.size()), 4}),
                &output_t));
        auto output = output_t->matrix<float>();
        for (auto row = 0; row < summary_entry_list.size(); row++) {
          const auto& entry = summary_entry_list[row];
          output(row, 0) = entry.value;
          output(row, 1) = entry.weight;
          output(row, 2) = entry.min_rank;
          output(row, 3) = entry.max_rank;
        }
      }
    };
    // TODO(tanzheny): comment on the magic number.
    const int64_t kCostPerUnit = 500 * batch_size;
    const DeviceBase::CpuWorkerThreads& worker_threads =
        *context->device()->tensorflow_cpu_worker_threads();
    Shard(worker_threads.num_threads, worker_threads.workers, num_features_,
          kCostPerUnit, do_quantile_summary_gen);
  }