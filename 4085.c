Status SetOpDevice(EagerContext& ctx, EagerOperation* op, Device** device) {
  // Here in local execute, set preferred device to be on the local task to
  // avoid placing op on a remote device with higher priority.
  const DeviceNameUtils::ParsedName& preferred_device =
      DeviceNameUtils::HasSomeDetails(op->GetDeviceParsedName())
          ? op->GetDeviceParsedName()
          : DeviceNameUtils::AddressSpace(ctx.HostCPUParsedName());
  // Note: We use the unwrapped op for inferring the device.
  // Without this, when wrapping CPU-only ops like RangeDataset we would
  // place the wrapped op on a GPU (if one is available) which leads to
  // errors because placer pins the function output nodes to GPU thereby
  // forcing a H2D copy of the dataset variant which is not supported.
  auto ndef = op->MutableAttrs()->BuildNodeDef();
#ifdef INTEL_MKL
  if (IsMKLEnabled() &&
      absl::StartsWith(op->Name(), mkl_op_registry::kMklOpPrefix)) {
    GetMKLNodeDef(&ndef);
  }
#endif  // INTEL_MKL

  TF_RETURN_IF_ERROR(ctx.SelectDevice(preferred_device, ndef, device));

  VLOG(1) << "PreferredDevice " << op->Name() << ": " << preferred_device;
  VLOG(1) << "Placer place op [" << op->Name()
          << "] on device: " << (*device)->name();
  VLOG(4) << "Available kernels for " << op->Name() << " are"
          << KernelsRegisteredForOp(op->Name());
  op->SetDevice(*device);
  return Status::OK();
}