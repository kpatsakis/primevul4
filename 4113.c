Status CreateUnshapedOutput(
    const KernelAndDevice& kernel, const int output_num, Device* output_device,
    const DataType& output_dtype,
    const absl::optional<EagerFunctionParams>& eager_func_params,
    EagerContext* ctx, TensorHandle** output) {
#if defined(IS_MOBILE_PLATFORM)
  return errors::Unimplemented(
      "Remote outputs are not available on mobile devices.");
#else  // !IS_MOBILE_PLATFORM
  int64_t op_id;
  if (eager_func_params.has_value()) {
    op_id = eager_func_params.value().op_id;
  } else {
    return errors::InvalidArgument(
        "Unable to find a remote op id for a remote output of ", kernel.name());
  }
  string remote_task;
  if (!DeviceNameUtils::GetTaskName(output_device->parsed_name(),
                                    &remote_task)) {
    return errors::InvalidArgument(
        "Unable to find remote task corresponding to device ",
        output_device->name());
  }
  if (ctx->RemoteMgr()->IsMaster()) {
    *output = TensorHandle::CreateUnshapedRemoteHandle(
        op_id, output_num, remote_task, output_dtype, output_device, ctx);
  } else {
    *output = TensorHandle::CreateLazyRemoteHandle(op_id, output_num,
                                                   output_dtype, output_device,
                                                   /*is_ready=*/false, ctx);
  }
  return Status::OK();
#endif  // !IS_MOBILE_PLATFORM
}