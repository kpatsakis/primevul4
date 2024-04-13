Status GetKernelOutputs(
    std::vector<EagerKernelRet>* outputs, int num_outputs,
    TensorHandle** retvals, EagerContext* ctx, KernelAndDevice* kernel,
    const absl::optional<EagerFunctionParams>& eager_func_params) {
  for (int i = 0, end = num_outputs; i < end; ++i) {
    if (retvals[i] == nullptr) {
      EagerKernelRet& ret = (*outputs)[i];
      Device* output_device = ctx->CanonicalDevice(kernel->OutputDevice(i));
      if (ret.index() == 0) {
        retvals[i] = TensorHandle::CreateLocalHandle(
            std::move(absl::get<Tensor>(ret)),
            /* d= */ output_device,
            /* op_device= */ kernel->device(),
            /* resource_device= */ kernel->OutputResourceDevice(i), ctx);
      } else {
        const DataTypeVector& output_dtypes = kernel->output_dtypes();
        TF_RETURN_IF_ERROR(
            CreateUnshapedOutput(*kernel, i, output_device, output_dtypes[i],
                                 eager_func_params, ctx, &retvals[i]));
#if !defined(IS_MOBILE_PLATFORM)
        TF_RETURN_IF_ERROR(
            retvals[i]->SetRemoteShape(absl::get<TensorShape>(ret),
                                       output_device, ctx->GetContextViewId()));
#endif  // IS_MOBILE_PLATFORM
      }
    } else {
      if (!kernel->IsFunction() &&
          TF_PREDICT_FALSE(kernel->device() != retvals[i]->op_device())) {
        return errors::Internal(
            "Kernel output tensor handle has a different op device than the "
            "kernel. This should never happen.");
      }
      if (TF_PREDICT_FALSE(ctx->CanonicalDevice(kernel->OutputDevice(i)) !=
                           retvals[i]->device())) {
        return errors::Internal(
            "Kernel output tensor handle locates on a different device than "
            "the specified kernel output device. This should never happen.");
      }

      EagerKernelRet& ret = (*outputs)[i];
      if (ret.index() == 0) {
        TF_RETURN_IF_ERROR(retvals[i]->SetTensor(
            std::move(absl::get<Tensor>(ret)),
            ctx->CanonicalDevice(kernel->OutputDevice(i))));
      } else {
#if defined(IS_MOBILE_PLATFORM)
        return errors::Unimplemented(
            "Remote outputs are not available on mobile devices.");
#else  // !IS_MOBILE_PLATFORM
        TF_RETURN_IF_ERROR(retvals[i]->SetRemoteShape(
            absl::get<TensorShape>(ret), retvals[i]->device(),
            ctx->GetContextViewId()));
#endif  // !IS_MOBILE_PLATFORM
      }
    }
  }
  return Status::OK();
}