Status EagerRemoteExecute(EagerOperation* op, TensorHandle** retvals,
                          int* num_retvals) {
  EagerContext& ctx = op->EagerContext();

  // TODO(fishx): Remove following code when lazy tensor copy is ready.
  if (op->Device() == kVariantDeviceNull) {
    tensorflow::Device* device = nullptr;
    string device_name = op->DeviceName();
    TF_RETURN_IF_ERROR(ctx.FindDeviceFromName(device_name.c_str(), &device));
    op->SetDevice(device);
  }

  core::RefCountPtr<eager::EagerClient> eager_client;
  uint64 context_id = ctx.GetContextId();
  TF_RETURN_IF_ERROR(ctx.GetClient(op->GetDeviceParsedName(), &eager_client));
  string remote_task;
  if (!DeviceNameUtils::GetTaskName(op->GetDeviceParsedName(), &remote_task)) {
    return errors::InvalidArgument(
        "Unable to find remote task corresponding to device ",
        op->DeviceName());
  }

  std::unique_ptr<eager::EnqueueRequest> request(new eager::EnqueueRequest);
  request->set_context_id(context_id);

  eager::Operation* remote_op = request->add_queue()->mutable_operation();

  tensorflow::Device* op_device = absl::get<Device*>(op->Device());
  {
    profiler::TraceMe activity("CopyInputToExpectedDevice",
                               profiler::TraceMeLevel::kInfo);
    const bool is_function = op->is_function();
    const absl::InlinedVector<TensorHandle*, 4>* inputs;
    TF_RETURN_IF_ERROR(op->TensorHandleInputs(&inputs));
    for (int i = 0, end = inputs->size(); i < end; i++) {
      tensorflow::TensorHandle* input = (*inputs)[i];
      tensorflow::Device* input_device = input->device();
      tensorflow::Device* input_device_or_cpu = input->DeviceOrHostCPU(ctx);
      const string* input_device_name = &input_device_or_cpu->name();
      bool serialize_resource_dtype_and_shape = false;
      if (op_device != input_device &&
          // If the expected and actual devices are on the same task, don't
          // explicitly copy, and instead depend on the copy to happen locally
          // when the op is executed on the device.
          !ctx.OnSameTask(op_device, input_device)) {
        if (!is_function || input_device_or_cpu->IsLocal()) {
          tensorflow::Device* remote_cpu_device;
          TF_RETURN_IF_ERROR(
              ctx.CPUDeviceOnTask(op_device, &remote_cpu_device));
          // Always copy to the remote CPU so that the actual device can be
          // correctly determined after the kernel is selected/instantiated,
          // since the op might have its inputs on host memory.
          TensorHandle* handle = input;
          Device* handle_device = handle->DeviceOrHostCPU(ctx);
          // If the input is already on the right device, then nothing to do.
          if (remote_cpu_device != handle_device) {
            VLOG(6) << "remote_cpu_device != handle_device";
            TF_RETURN_IF_ERROR(CopyInputToExpectedDevice(
                &ctx, op, op_device, handle, i, handle_device,
                remote_cpu_device, &handle));
            op->UpdateInput(i, handle);
            input = handle;
            input_device = remote_cpu_device;
            input_device_name = &remote_cpu_device->name();
            // Unref handle since it has a ref as an input now
            handle->Unref();
          }
        } else {
          serialize_resource_dtype_and_shape =
              (input->dtype == DT_RESOURCE) &&
              (!input->HasResourceShapeMirror(op_device,
                                              ctx.GetContextViewId()));
        }
      }
      auto* input_handle = remote_op->add_op_inputs()->mutable_remote_handle();
      // For a remote component function, a function execution request and an
      // input generation request may come from different workers. We need to
      // guarantee that the input generation request is processed before the
      // function execution request, so wait until the remote input is ready
      // before sending it to the multi-device function device.
      const bool wait_until_ready = op->is_function();
      TF_RETURN_IF_ERROR(ctx.RemoteMgr()->SerializeRemoteTensorHandle(
          input, wait_until_ready, input_handle, input_device,
          *input_device_name, serialize_resource_dtype_and_shape));
      if (!input_handle->resource_dtypes_and_shapes().empty()) {
        TF_RETURN_IF_ERROR(
            input->AddResourceShapeMirror(op_device, input_handle->op_id(),
                                          input_handle->output_num(), &ctx));
      }
    }
  }

  PrepareRemoteOp(remote_op, op);

  DataTypeVector output_dtypes;
  TF_RETURN_IF_ERROR(GetOutputDTypes(op, &output_dtypes));

  const size_t num_outputs = output_dtypes.size();
  if (num_outputs != *num_retvals) {
    return errors::InvalidArgument(
        "num_retvals does not match expected output dtypes");
  }
  *num_retvals = num_outputs;

  const tensorflow::uint64 id = remote_op->id();
  for (size_t i = 0; i < num_outputs; ++i) {
    // TODO(nareshmodi): Change the callback to instead add the decref to a
    // list of pending decrefs that we can send as a batch with the next
    // execute.

    // The device_ and resource_device_ of this TensorHandle might be
    // incorrect. For multi-device functions, we don't know the output device
    // until the function is instantiated on a remote worker. Luckily, we don't
    // need to know the correct remote device here. We just need to know that it
    // is remote. If we need copy this tensor to this process or run any ops
    // which take this tensor as an input, block until the correct device is
    // set.
    const bool unknown_device = op->is_function();
    retvals[i] = TensorHandle::CreateUnshapedRemoteHandle(
        id, i, remote_task, output_dtypes[i], op_device, &ctx, unknown_device);
  }

  // Store the data type and shape of a remote resource variable on the
  // corresponding remote TensorHandle (output of 'VarHandleOp').
  // If the variable is an input of a remote function, the function may need
  // the type and shape during function instantiation. Store the type and
  // shape on eager master and sent them to the default function device along
  // with the EnqueueRequest.
  TF_RETURN_IF_ERROR(
      StoreResourceDtypesAndShapes(*remote_op, output_dtypes, retvals));

  auto& executor = op->Executor();
  VLOG(4) << "Execute remote eager op: " << op->Name()
          << " (is async?: " << executor.Async() << ").";

  const absl::InlinedVector<TensorHandle*, 4>* inputs;
  TF_RETURN_IF_ERROR(op->TensorHandleInputs(&inputs));

  std::unique_ptr<EagerNode> node(new eager::RemoteExecuteNode(
      &op->EagerContext(), std::move(request), op_device,
      ctx.GetContextViewId(), eager_client.get(), op->GetCancellationManager(),
      op->MutableAttrs()->BuildNodeDef(), op->EagerContext().FuncLibDef(),
      *inputs, {retvals, num_outputs}));

  if (op->EagerContext().LogDevicePlacement() || VLOG_IS_ON(1)) {
    string msg = strings::StrCat(
        "Executing op ", op->Name(), " on task ",
        DeviceNameUtils::ParsedNameToString(op->GetDeviceParsedName()));
    if (!logging::LogToListeners(msg)) {
      LOG(INFO) << msg;
    }
  }

  Status s = executor.AddOrExecute(std::move(node));
  // Since the operation failed, we need to Unref any outputs that were
  // allocated.
  if (!s.ok()) {
    for (size_t i = 0; i < num_outputs; ++i) {
      retvals[i]->Unref();
      // Ensure that any smart pointers created to wrap results become noops
      // rather than operating on invalid memory.
      retvals[i] = nullptr;
    }
  }

  return s;
}