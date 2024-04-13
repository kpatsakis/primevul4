Status AddOrExecuteNode(core::RefCountPtr<KernelAndDevice> kernel,
                        EagerOperation* op, TensorHandle** retvals) {
  EagerExecutor& executor = op->Executor();
  EagerContext& ctx = op->EagerContext();
  GraphCollector* graph_collector = nullptr;
  if (ctx.ShouldStoreGraphs()) {
    graph_collector = ctx.GetGraphCollector();
  }
  const int num_outputs = kernel->num_outputs();
  absl::optional<EagerFunctionParams> eager_func_params =
      op->eager_func_params();
  if (kernel->IsCrossProcess() && !eager_func_params.has_value()) {
    // Create an eager op id for a cross-process function if not exist.
#if defined(IS_MOBILE_PLATFORM)
    return errors::Unimplemented(
        "Cross-process functions are not supported on mobile devices.");
#else  // !IS_MOBILE_PLATFORM
    const int64_t op_id = ctx.RemoteMgr()->NextOpId();
    eager_func_params = EagerFunctionParams{op_id, /*step_id=*/absl::nullopt};
#endif  // !IS_MOBILE_PLATFORM
  }
  if (executor.Async()) {
    const DataTypeVector& output_dtypes = kernel->output_dtypes();
    for (int i = 0, end = num_outputs; i < end; ++i) {
      Device* output_device = ctx.CanonicalDevice(kernel->OutputDevice(i));
      if (output_device == nullptr || output_device->IsLocal()) {
        retvals[i] = TensorHandle::CreateEmptyLocalHandle(
            /* d= */ output_device, /* op_device= */ kernel->device(),
            /* resource_device= */ kernel->OutputResourceDevice(i),
            output_dtypes[i], &ctx);
      } else {
        TF_RETURN_IF_ERROR(
            CreateUnshapedOutput(*kernel, i, output_device, output_dtypes[i],
                                 eager_func_params, &ctx, &retvals[i]));
      }
    }
    const absl::InlinedVector<TensorHandle*, 4>* inputs;
    TF_RETURN_IF_ERROR(op->TensorHandleInputs(&inputs));
    auto node = absl::make_unique<AsyncExecuteNode>(
        &ctx, *inputs, eager_func_params, std::move(kernel), graph_collector,
        op->GetCancellationManager(),
        absl::Span<TensorHandle*>(retvals, num_outputs), op->GetStackTrace());
    // Release the inputs from the eager operation since the AsyncExecuteNode
    // would have taken ownership. This allows the inputs to be forwarded if
    // possible.
    op->Clear();
    // For async mode, execution order will make sure that all
    // input handles are ready before executing them.
    // TODO(b/137118203): Consider executing "cheap" kernels inline for
    // performance.
    return executor.AddOrExecute(std::move(node));
  } else {
    for (int i = 0, end = num_outputs; i < end; ++i) {
      retvals[i] = nullptr;
    }
    const absl::InlinedVector<TensorHandle*, 4>* inputs;
    TF_RETURN_IF_ERROR(op->TensorHandleInputs(&inputs));
    ExecuteNode node(&ctx, *inputs, eager_func_params, kernel, graph_collector,
                     op->GetCancellationManager(),
                     {retvals, static_cast<size_t>(num_outputs)},
                     op->GetStackTrace());
    Status s = executor.SyncExecute(&node);
    // We release the inputs AFTER executing the operation in sync mode since
    // ExecuteNode does not increment the reference count and thus does not have
    // ownership of the inputs while executing.
    op->Clear();
    return s;
  }
}