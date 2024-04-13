void EagerKernelExecuteAsync(
    EagerContext* ctx, const absl::InlinedVector<TensorHandle*, 4>& op_inputs,
    const absl::optional<EagerFunctionParams>& eager_func_params,
    const core::RefCountPtr<KernelAndDevice> kernel,
    GraphCollector* graph_collector, CancellationManager* cancellation_manager,
    TensorHandle** retvals, int num_outputs, StatusCallback done) {
  auto inputs = std::make_shared<ExecuteNodeArgs>(op_inputs.size());
  auto outputs = std::make_shared<std::vector<EagerKernelRet>>(1);

  Status s = inputs->Init(ctx, op_inputs, kernel);
  if (!s.ok()) {
    done(s);
    return;
  }
  CoordinationServiceAgent* coord_agent = nullptr;
#if !defined(IS_MOBILE_PLATFORM)
  if (ctx->GetDistributedManager() != nullptr)
    coord_agent = ctx->GetDistributedManager()->GetCoordinationServiceAgent();
#endif  // !IS_MOBILE_PLATFORM

  kernel->Ref();  // Ownership of reference is transferred to the callback
  kernel->RunAsync(
      ctx->StepContainer(), *inputs, outputs.get(), cancellation_manager,
      eager_func_params, coord_agent,
      [retvals, inputs, outputs, num_outputs, ctx, graph_collector,
       eager_func_params, kernel_raw = kernel.get(),
       done = std::move(done)](const Status& s) {
        auto wrapped_done = [&](const Status& s) {
          kernel_raw->Unref();
          done(s);
        };
        if (!s.ok()) {
          wrapped_done(s);
          return;
        }
        if (graph_collector != nullptr) {
          CollectGraphs(ctx);
        }
        DCHECK_EQ(num_outputs, outputs->size());
        wrapped_done(GetKernelOutputs(outputs.get(), num_outputs, retvals, ctx,
                                      kernel_raw, eager_func_params));
      });
}