void EagerLocalExecuteAsync(EagerOperation* op, TensorHandle** retvals,
                            int* num_retvals, StatusCallback done) {
  if (!op->IsLocal()) {
    done(errors::InvalidArgument(
        "Remote execution is not supported in async EagerLocalExecuteAsync"));
    return;
  }

  profiler::ScopedMemoryDebugAnnotation op_annotation(
      op->op_name(), op->eager_func_params().has_value()
                         ? op->eager_func_params().value().step_id.value_or(0)
                         : 0);
  profiler::TraceMe activity(
      [&] { return absl::StrCat("EagerLocalExecuteAsync: ", op->Name()); },
      profiler::TraceMeLevel::kInfo);
  EagerContext& ctx = op->EagerContext();

  core::RefCountPtr<KernelAndDevice> kernel;
  Status s = GetOrCreateKernelAndDevice(op, retvals, num_retvals, &kernel);
  if (!s.ok()) {
    done(s);
    return;
  }

  int num_outputs = kernel->num_outputs();
  s = ValidateInputTypeAndPlacement(&ctx, op, kernel);
  if (!s.ok()) {
    done(s);
    return;
  }

  if (ctx.LogDevicePlacement() || VLOG_IS_ON(1)) {
    string msg = strings::StrCat("Executing op ", op->Name(), " in device ",
                                 kernel->device()->name());
    if (!logging::LogToListeners(msg)) {
      LOG(INFO) << msg;
    }
  }

  GraphCollector* graph_collector = nullptr;
  if (ctx.ShouldStoreGraphs()) {
    graph_collector = ctx.GetGraphCollector();
  }

  for (int i = 0, end = num_outputs; i < end; ++i) {
    const DataTypeVector& output_dtypes = kernel->output_dtypes();
    retvals[i] = TensorHandle::CreateEmptyLocalHandle(
        /* d= */ ctx.CanonicalDevice(kernel->OutputDevice(i)),
        /* op_device= */ kernel->device(),
        /* resource_device= */ kernel->OutputResourceDevice(i),
        output_dtypes[i], &ctx);
  }

  const absl::InlinedVector<TensorHandle*, 4>* inputs;
  s = op->TensorHandleInputs(&inputs);
  if (!s.ok()) {
    done(s);
    return;
  }
  EagerKernelExecuteAsync(
      &ctx, *inputs, op->eager_func_params(), std::move(kernel),
      graph_collector, op->GetCancellationManager(), retvals, num_outputs,
      [op, num_outputs, retvals, done = std::move(done)](const Status& s) {
        op->Clear();
        // Since the operation failed, we need to Unref any outputs if they were
        // allocated.
        if (!s.ok()) {
          for (int i = 0, end = num_outputs; i < end; ++i) {
            if (retvals[i] != nullptr) {
              retvals[i]->Unref();
              retvals[i] = nullptr;
            }
          }
        }
        done(s);
      });
}