Status GetOrCreateKernelAndDevice(
    EagerOperation* op, TensorHandle** retvals, int* num_retvals,
    core::RefCountPtr<KernelAndDevice>* out_kernel) {
  EagerContext& ctx = op->EagerContext();
  Device* device = absl::get<Device*>(op->Device());

  // Set the EagerOperation's device prior to extracting the input_dev_ptrs to
  // avoid any redundant H2D/D2H copies.
  if (device == nullptr && !op->is_function()) {
    Fprint128 device_cache_key = GetDeviceCacheKey(op, ctx);
    device = ctx.GetCachedDevice(device_cache_key);
    if (device == nullptr) {
      TF_RETURN_IF_ERROR(SetOpDevice(ctx, op, &device));
      ctx.AddDeviceToCache(device_cache_key, device);
    } else {
      op->SetDevice(device);
    }
  }

  // Save the original value of reuse_rendezvous_for_functions from the context.
  bool reuse_rendezvous_for_functions_original_value =
      ctx.GetReuseRendezvousForFunctions();
  // When running in eager_op_as_function mode Send/Recv ops need to be
  // placed on the same rendezvous to match the behaviour of eager mode.
  bool reuse_rendezvous_for_functions =
      (ctx.RunEagerOpAsFunction() && !op->is_function()) ||
      reuse_rendezvous_for_functions_original_value;

  std::vector<Device*> input_dev_ptrs;
  absl::flat_hash_map<string, const std::vector<string>*> composite_devices;
  std::unordered_map<int, DtypeAndPartialTensorShape>
      input_resource_variable_dtypes_and_shapes;
  if (op->is_function() || ctx.RunEagerOpAsFunction()) {
    profiler::TraceMe activity("EagerCopyToDevice",
                               profiler::TraceMeLevel::kInfo);
    input_dev_ptrs.reserve(op->Inputs().size());
    const absl::InlinedVector<TensorHandle*, 4>* inputs;
    TF_RETURN_IF_ERROR(op->TensorHandleInputs(&inputs));
    for (int i = 0, end = inputs->size(); i < end; ++i) {
      TensorHandle* input = (*inputs)[i];

      Device* input_device;
      TF_RETURN_IF_ERROR(GetDeviceForInput(*op, ctx, input, &input_device));
      VLOG(1) << op->Name() << ":input:" << i << " " << input_device->name();
      input_dev_ptrs.push_back(input_device);
      CompositeDevice* composite_device = nullptr;
      if (ctx.FindCompositeDeviceFromName(input_device->name(),
                                          &composite_device)
              .ok()) {
        composite_devices[input_device->name()] =
            composite_device->underlying_devices();
      }
      if (input->dtype == DT_RESOURCE) {
        // We only care about data type and shape for resource variable inputs.
        // But we have no way to tell if input is resource variable (other than
        // looking it up in ResourceMgr, which is slow). So we just get
        // resource_dtypes_and_shapes for all DT_RESOURCE inputs. If
        // resource_dtypes_and_shapes is not empty, take the first element.
        std::vector<DtypeAndPartialTensorShape> resource_dtypes_and_shapes;
        TF_RETURN_IF_ERROR(input->GetResourceHandleDtypesAndShapes(
            &resource_dtypes_and_shapes));
        if (!resource_dtypes_and_shapes.empty()) {
          const DtypeAndPartialTensorShape& dtype_and_shape =
              resource_dtypes_and_shapes.at(0);
          input_resource_variable_dtypes_and_shapes[i] = dtype_and_shape;
        }
      }
    }
  }

  TF_ASSIGN_OR_RETURN(
      Fprint128 cache_key,
      GetKernelCacheKey(*op, op->MutableAttrs()->CacheKey(op->DeviceName()),
                        input_dev_ptrs,
                        input_resource_variable_dtypes_and_shapes));
  core::RefCountPtr<KernelAndDevice> kernel = ctx.GetCachedKernel(cache_key);
  AbstractOperationPtr wrapped_op_releaser;
  // We can eliminate some overhead by running simple functions using regular
  // CallOp kernel. However, it is tricky to figure out which functions should
  // be run using CallOp. Also, currently CallOp runs neither optimization
  // passes (needed for TPU/XLA) nor grappler.
  // Here are some cases where a function should be run in multi-device mode:
  //  - Function takes at least two resources on different devices.
  //  - Function takes a resource on deviceA and a body op explicitly placed
  //  on deviceB.
  //  - Function has a colocation constraint.
  //  - Function has an explicit device annotation (which might not be using
  //    full canonical device name) different from op_device. Note that false
  //    positives are ok.
  //  - Function has a node or a (node) attribute that can potentially make
  //    the function multi-device after a rewrite pass (e.g. various XLA/TPU
  //    special nodes and attributes)
  if (kernel == nullptr) {
    VLOG(2) << "Creating new kernel for " << op->Name() << " on device "
            << DeviceNameOrUnspecified(absl::get<Device*>(op->Device()));
    bool run_function_with_flr = false;
    bool function_outputs_on_op_device = false;
    absl::optional<string> xla_compile_device_type;
    if (op->is_function()) {
      bool compile_with_xla;
      TF_RETURN_IF_ERROR(MustCompileWithXLA(op, ctx, &compile_with_xla));
      if (compile_with_xla) {
        if (ctx.JitCompileRewrite()) {
          xla_compile_device_type = op->GetDeviceParsedName().type;
          run_function_with_flr = true;
        } else {
          // Note that it is not ideal, but currently correct, to set this
          // attribute after computing the kernel cache key above.
          // Note: If the attribute is already set to true, this is a noop.
          op->MutableAttrs()->Set(kXlaMustCompileAttr, true);
        }
      } else {
        run_function_with_flr = true;
      }
      GetFuncAttr(op, ctx, kOutputsOnOpDevice, &function_outputs_on_op_device)
          .IgnoreError();
    }

    VLOG(2) << op->Name() << " function_outputs_on_op_device: "
            << function_outputs_on_op_device;
    if (device == nullptr) {
      TF_RETURN_IF_ERROR(SetOpDevice(ctx, op, &device));
    } else {
      VLOG(1) << "Device for [" << op->Name()
              << "] already set to: " << device->name();
    }

    // Note: We wrap the eager op AFTER the device has been inferred to ensure
    // that placement of the NodeDef in the function is exactly the same as in
    // eager mode. This is specially important for cases where the
    // preferred device is not the actual device on which the op is run.
    // E.g. the preferred device for a `RangeDataset` op could be set to `GPU`
    // but `ctx->SelectDevice` would still place it on CPU. Placer on the other
    // hand would throw an error.
    //
    // Note: The wrapped function is never jit compiled but rather run via the
    // FLR. This is needed because certain ops e.g. `VarHandleOp` can not be
    // jit compiled. Ideally we would run this via the jit compiled path and
    // expect unsupported ops to be outside compiled but that is not supported
    // on GPUs right now.
    bool allow_small_function_optimizations = false;
    bool int_args_and_retvals_on_device = false;
    bool allow_control_flow_sync_execution = false;
    // TODO(b/176491312): Remove this if shape inference on import flag is
    // removed.
    bool shape_inference_on_tfe_dialect_import = true;
    if (ctx.RunEagerOpAsFunction() && !op->is_function()) {
      EagerOperation* wrapped_op = nullptr;
      TF_RETURN_IF_ERROR(ValidateOp(op));
      TF_RETURN_IF_ERROR(WrapInCallOp(op, &wrapped_op));
      DCHECK(wrapped_op);
      DCHECK(wrapped_op->is_function());
      wrapped_op_releaser.reset(wrapped_op);
      run_function_with_flr = true;
      allow_small_function_optimizations = true;
      allow_control_flow_sync_execution = true;
      shape_inference_on_tfe_dialect_import = false;
      int_args_and_retvals_on_device = IntArgsAndRetvalsOnDevice(op);
      op = wrapped_op;
    }
    const NodeDef& ndef = op->MutableAttrs()->BuildNodeDef();

    FunctionLibraryRuntime* flr =
        device == nullptr ? nullptr : ctx.func_lib(device);
    if (device != nullptr && flr == nullptr) {
      return errors::NotFound(
          "Unable to find a FunctionLibraryRuntime corresponding to device ",
          device->name());
    }
    auto runner = (flr != nullptr && flr->runner() != nullptr) ? flr->runner()
                                                               : ctx.runner();
    GraphCollector* graph_collector = nullptr;
    if (ctx.ShouldStoreGraphs()) {
      graph_collector = ctx.GetGraphCollector();
    }
    // Treat the function as multi_device only when we are not compiling
    // it wholly with XLA. When compiling wholly with XLA, flr->CreateKernel
    // will create an XlaLaunchOp kernel to compile and run the function.
    if (run_function_with_flr) {
      // Multi-device functions don't use the rendezvous from eager context.
      // If we use that rendezvous, multiple concurrent calls to the same
      // function will likely result in collisions. However, this also means
      // that we don't support legitimate sending/receiving across function
      // boundary.
      VLOG(2) << "Running " << ndef.op() << " using multi-device function. "
              << "Full node_def=" << ndef.DebugString();
      std::function<int64_t()> get_op_id = nullptr;
#if !defined(IS_MOBILE_PLATFORM)
      get_op_id = [&ctx]() { return ctx.RemoteMgr()->NextOpId(); };
#endif  // IS_MOBILE_PLATFORM

      ctx.reuse_rendezvous_for_functions_mu()->lock();
      ctx.SetReuseRendezvousForFunctions(reuse_rendezvous_for_functions);
      auto rendezvous_creator = ctx.RendezvousCreator();
      ctx.SetReuseRendezvousForFunctions(
          reuse_rendezvous_for_functions_original_value);
      ctx.reuse_rendezvous_for_functions_mu()->unlock();
      kernel.reset(new KernelAndDeviceFunc(
          flr, ctx.pflr(), std::move(input_dev_ptrs),
          std::move(composite_devices),
          std::move(input_resource_variable_dtypes_and_shapes), runner,
          ctx.GetCollectiveExecutorHandle(), ctx.HostCPU(), op->Name(),
          function_outputs_on_op_device, allow_small_function_optimizations,
          allow_control_flow_sync_execution,
          shape_inference_on_tfe_dialect_import, int_args_and_retvals_on_device,
          xla_compile_device_type, std::move(rendezvous_creator), get_op_id));
    } else {
      VLOG(2) << "Running " << ndef.op() << " using op kernel. "
              << ". Full node_def=" << ndef.DebugString();
      kernel.reset(new KernelAndDeviceOp(
          ctx.GetRendezvous(), ctx.LogMemory(), flr, runner,
          ctx.GetCollectiveExecutorHandle(), ctx.HostCPU()));
    }

    TF_RETURN_IF_ERROR(
        kernel->Init(ctx.LogDevicePlacement(), ndef, graph_collector));

    if (op->is_function()) {
      ctx.AddKernelToCache(cache_key, kernel.get());
    } else {
      // Exclude tf.data op kernels from being cached. The reason for this is
      // that tf.data op kernels that accept a user-defined function will have a
      // unique cache key every time they are executed (because the user-defined
      // function is traced every time). Caching such kernels provides no
      // benefit and in some cases results in linear memory growth of use
      // programs that build input pipeline graphs in a loop.
      const OpDef* op_def;
      TF_RETURN_IF_ERROR(OpDefForOp(op->Name().data(), &op_def));
      if (KernelCacheEnabled(*op_def)) {
        ctx.AddKernelToCache(cache_key, kernel.get());
      }
    }
  }

  int num_outputs = kernel->num_outputs();
  if (num_outputs > *num_retvals) {
    return errors::InvalidArgument("Expecting ", num_outputs,
                                   " outputs, but *num_retvals is ",
                                   *num_retvals);
  }
  *num_retvals = num_outputs;

  kernel->Ref();  // Ownership of reference is passed to out_kernel.
  out_kernel->reset(kernel.get());
  return Status::OK();
}