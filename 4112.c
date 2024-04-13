Status EagerKernelExecute(
    EagerContext* ctx, const absl::InlinedVector<TensorHandle*, 4>& op_inputs,
    const absl::optional<EagerFunctionParams>& eager_func_params,
    const core::RefCountPtr<KernelAndDevice>& kernel,
    GraphCollector* graph_collector, CancellationManager* cancellation_manager,
    absl::Span<TensorHandle*> retvals,
    const absl::optional<ManagedStackTrace>& stack_trace) {
  profiler::TraceMe activity("EagerKernelExecute",
                             profiler::TraceMeLevel::kInfo);
  std::vector<EagerKernelRet> outputs(1);

  ExecuteNodeArgs inputs(op_inputs.size());
  TF_RETURN_IF_ERROR(inputs.Init(ctx, op_inputs, kernel));
  // TODO(apassos) figure out how to record stats for ops which are a part of
  // functions.
  // TODO(b/111859745): When we support recovering from kernel/device errors, we
  // would need to call XlaDevice::EnsureDeviceContextOk() before using an XLA
  // device. We don't call it now because it is an unneeded overhead (it
  // acquires a lock) and we can't recover from errors anyway.
  ScopedStepContainer* container = ctx->StepContainer();
  CoordinationServiceAgent* coord_agent = nullptr;
#if !defined(IS_MOBILE_PLATFORM)
  if (ctx->GetDistributedManager() != nullptr)
    coord_agent = ctx->GetDistributedManager()->GetCoordinationServiceAgent();
#endif  // !IS_MOBILE_PLATFORM
  TF_RETURN_IF_ERROR(kernel->Run(container, inputs, &outputs,
                                 cancellation_manager, eager_func_params,
                                 stack_trace, coord_agent));
  if (graph_collector != nullptr) {
    CollectGraphs(ctx);
  }

  if (TF_PREDICT_FALSE(retvals.size() != outputs.size())) {
    return errors::Internal(
        "EagerKernelExecute returns a list of ", outputs.size(),
        " tensors but ", retvals.size(),
        " is expected. This should never "
        "happen. Please file a bug with the TensorFlow team.");
  }
  return GetKernelOutputs(&outputs, retvals.size(), retvals.data(), ctx,
                          kernel.get(), eager_func_params);
}