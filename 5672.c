Status CopyVariable(int output_idx, OpKernelContext* ctx, const Tensor* t) {
  Tensor* output;
  Notification n;
  Status status;
  AllocatorAttributes attr;
  if (t->dtype() == DT_VARIANT) {
    attr.set_on_host(true);
  }
  TF_RETURN_IF_ERROR(
      ctx->allocate_output(output_idx, t->shape(), &output, attr));
  if (t->dtype() == DT_VARIANT) {
    output->flat<Variant>() = t->flat<Variant>();
  } else if (ctx->op_device_context() != nullptr) {
    // TODO(apassos): remove the down_cast by just returning Device* from
    // OpKernelContext
    Device* device = down_cast<Device*>(ctx->device());
    ctx->op_device_context()->CopyTensorInSameDevice(
        t, device, output, [&n, &status](const Status& s) {
          status = s;
          n.Notify();
        });
    n.WaitForNotification();
    return status;
  } else {
    switch (t->dtype()) {
#define HANDLER(type)                       \
  case DataTypeToEnum<type>::value:         \
    output->flat<type>() = t->flat<type>(); \
    break;
      TF_CALL_ALL_TYPES(HANDLER);
#undef HANDLER
      default:
        return errors::Internal("Unsupported dtype", t->dtype());
    }
  }
  return Status::OK();
}