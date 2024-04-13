Status EagerExecute(EagerOperation* op, TensorHandle** retvals,
                    int* num_retvals) {
  profiler::TraceMe activity([&] {
    return ::tensorflow::profiler::TraceMeEncode(
        "EagerExecute",
        {{"eager_op", op->Name()}, {"is_func", op->is_function()}});
  });

  if (!op->Executor().Async()) {
    VLOG(6) << "op: " << op->Name() << " is not Async.";
    if (!op->EagerContext()
             .GetGlobalRendezvousForFunctionLocalRendezvousStatus()
             .ok()) {
      VLOG(6) << "global_rendezvous_for_functions_ is in bad state. Resetting.";
      op->EagerContext().ResetGlobalRendezvousForFunction();
    }
    // In sync mode, always clear error to maintain the same behavior as before.
    // TODO(b/141004939): Remove this.
    op->Executor().ClearError();
  }

  std::unique_ptr<tensorflow::EagerOperation> out_op;
  TF_RETURN_IF_ERROR(EagerOpRewriteRegistry::Global()->RunRewrite(
      EagerOpRewriteRegistry::PRE_EXECUTION, op, &out_op));

  if (op->IsLocal()) {
    if (out_op) {
      op = out_op.get();
    }
    TF_RETURN_IF_ERROR(MaybePackInputTensor(op));
    return EagerLocalExecute(op, retvals, num_retvals);
  }

#if defined(IS_MOBILE_PLATFORM)
  return errors::Unimplemented(
      "Eager's remote execution is not available on mobile devices.");
#else   // !IS_MOBILE_PLATFORM
  if (out_op) {
    op = out_op.get();
  }
  return EagerRemoteExecute(op, retvals, num_retvals);
#endif  // !IS_MOBILE_PLATFORM
}