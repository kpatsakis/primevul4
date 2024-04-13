Status ValidateInputTypeAndPlacement(
    EagerContext* ctx, EagerOperation* op,
    const core::RefCountPtr<KernelAndDevice>& kernel) {
  profiler::TraceMe activity("ValidateInputTypeAndPlacement",
                             profiler::TraceMeLevel::kInfo);
  const int n_inputs = op->Inputs().size();
  if (kernel->num_inputs() != n_inputs) {
    return errors::InvalidArgument("expected ", kernel->num_inputs(),
                                   " inputs, got ", n_inputs);
  }
  const bool is_function = kernel->IsFunction();
  if (n_inputs > 0) {
    const DataType* input_types = &kernel->input_dtypes()[0];
    const absl::InlinedVector<TensorHandle*, 4>* handles;
    TF_RETURN_IF_ERROR(op->TensorHandleInputs(&handles));
    for (int i = 0; i < n_inputs; ++i) {
      TensorHandle* handle = (*handles)[i];
      Device* expected_device = kernel->InputDevice(i);
      if (!kernel->IsFunction() && handle->Type() == TensorHandle::PACKED) {
        // Extract a handle on the op device from a packed input.
        // This happens when a function is marked for XLA compilation.
        // MaybePackInputTensor guarantees that a primitive op has no packed
        // input at this point.
        for (int j = 0; j < handle->NumPackedHandles(); ++j) {
          TensorHandle* h = nullptr;
          TF_RETURN_IF_ERROR(handle->ExtractPackedHandle(j, &h));
          if ((h->op_device() != nullptr) &&
              (h->op_device()->name() == op->DeviceName())) {
            op->UpdateInput(i, h);
            handle = h;
            break;
          }
        }
      }
      Device* handle_device = handle->DeviceOrHostCPU(*ctx);
      const bool maybe_copy =
          !is_function || handle->Type() != TensorHandle::REMOTE;
      VLOG(6) << "!is_function: " << !is_function;
      VLOG(6) << "handle->Type(): " << handle->Type();
      // If the input is already on the right device, then nothing to do.
      if (expected_device != handle_device && maybe_copy) {
        TF_RETURN_IF_ERROR(CopyInputToExpectedDevice(ctx, op, kernel->device(),
                                                     handle, i, handle_device,
                                                     expected_device, &handle));
        op->UpdateInput(i, handle);
        // Unref handle since it has a ref as an input now
        handle->Unref();
      }
      if (handle->dtype != input_types[i]) {
        return errors::InvalidArgument(
            "cannot compute ", op->Name(), " as input #", i, "(zero-based)",
            " was expected to be a ", DataTypeString(input_types[i]),
            " tensor but is a ", DataTypeString(handle->dtype), " tensor");
      }
    }
  }
  return Status::OK();
}