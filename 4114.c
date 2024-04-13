Status MaybePackInputTensor(EagerOperation* op) {
  if (op->is_function() || op->EagerContext().RunEagerOpAsFunction()) {
    // Functions could take packed TensorHandles as inputs.
    return Status::OK();
  }
  EagerContext& ctx = op->EagerContext();
  const absl::InlinedVector<TensorHandle*, 4>* inputs;
  TF_RETURN_IF_ERROR(op->TensorHandleInputs(&inputs));
  for (int i = 0; i < inputs->size(); ++i) {
    TensorHandle* handle = (*inputs)[i];
    if (handle->Type() == TensorHandle::PACKED) {
      EagerOperation pack_op(&ctx);
      TF_RETURN_IF_ERROR(pack_op.Reset("Pack", /*device_name=*/nullptr,
                                       /*remote=*/false, /*executor=*/nullptr));
      pack_op.MutableAttrs()->Set("N", handle->NumPackedHandles());
      pack_op.MutableAttrs()->Set("T", handle->dtype);
      for (int i = 0; i < handle->NumPackedHandles(); ++i) {
        tensorflow::TensorHandle* h = nullptr;
        TF_RETURN_IF_ERROR(handle->ExtractPackedHandle(i, &h));
        TF_RETURN_IF_ERROR(pack_op.AddInput(h));
      }
      int num_retvals = 1;
      absl::FixedArray<tensorflow::TensorHandle*> retvals(num_retvals);
      TF_RETURN_IF_ERROR(
          EagerLocalExecute(&pack_op, retvals.data(), &num_retvals));
      tensorflow::TensorHandle* ret = retvals.at(0);
      op->UpdateInput(i, ret);
      ret->Unref();
    }
  }
  return Status::OK();
}