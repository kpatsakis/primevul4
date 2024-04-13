Status CopyInputToExpectedDevice(EagerContext* ctx, EagerOperation* op,
                                 Device* op_device,
                                 TensorHandle* handle,  // op->Inputs()[i]
                                 int i, Device* handle_device,
                                 Device* expected_input_device,
                                 TensorHandle** result) {
  VLOG(6) << "Expected input device: " << expected_input_device->name()
          << "; handle_device: " << handle_device->name();
  // Should only be called when these don't match
  DCHECK(expected_input_device != handle_device);
  *result = nullptr;
  const string& op_device_name = DeviceNameOrUnspecified(op_device);

  switch (ctx->GetDevicePlacementPolicy()) {
    case DEVICE_PLACEMENT_SILENT_FOR_INT32:
      // TODO(xpan): See if we could bubble python related error up
      // to python level.
      if (handle->dtype == DT_INT32) {
        // Note: enabling silent copies of int32 tensors to match behavior
        // of graph mode.
        break;
      }
      VLOG(6) << "DevicePlacementPolicy: DEVICE_PLACEMENT_SILENT_FOR_INT32 but "
                 "input type is not INT32.";
      TF_FALLTHROUGH_INTENDED;
    case DEVICE_PLACEMENT_EXPLICIT:
      // tf.identity is allowed to copy, as indicated in the error message
      // below.
      if (op->Name() == "Identity" ||
          op->Name() == "IdentityN"
          // Constants start on CPU:0 and are copied via EagerConst to the
          // current device.
          || op->Name() == "_EagerConst") {
        break;
      }
      return errors::InvalidArgument(
          "Tensors on conflicting devices:"
          " cannot compute ",
          op->Name(), " as input #", i, " was expected to be on ",
          expected_input_device->name(), " but is actually on ",
          handle_device->name(), " (operation running on ", op_device_name, ")",
          " Tensors can be copied explicitly using:"
          " `with tf.device(device_name): x = tf.identity(x)`"
          " or transparently copied by using"
          " tf.config.experimental.set_device_policy('silent')."
          " Copying tensors between devices may slow down your model");
    case DEVICE_PLACEMENT_WARN:
      LOG(WARNING) << "before computing " << op->Name() << " input #" << i
                   << " was expected to be on " << expected_input_device->name()
                   << " but is actually on " << handle_device->name()
                   << " (operation running on " << op_device_name
                   << "). This triggers a copy which can be a performance "
                      "bottleneck.";
      break;
    case DEVICE_PLACEMENT_SILENT:  // Do nothing.
      break;
  }
  // We are only here if the policy is warn or silent copies, so we should
  // trigger a copy.
  TensorHandle* result_handle = nullptr;
  profiler::TraceMe activity(
      [&] {
        return absl::StrCat("_Send input ", i, " from ", handle_device->name(),
                            " to ", expected_input_device->name());
      },
      profiler::TraceMeLevel::kInfo);
  Status status =
      EagerCopyToDevice(handle, ctx, &op->Executor(), expected_input_device,
                        /* mirror= */ true, &result_handle);
  activity.Stop();
  if (!status.ok()) {
    return Status(
        status.code(),
        absl::StrCat("Failed copying input tensor from ", handle_device->name(),
                     " to ", expected_input_device->name(), " in order to run ",
                     op->Name(), ": ", status.error_message()));
  }

  *result = result_handle;

  return Status::OK();
}