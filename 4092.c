bool IntArgsAndRetvalsOnDevice(EagerOperation* op) {
  // Most TF ops expect and generate int32 tensors on the host (or a TPU/XLA
  // device). This is not the case with IteratorGetNext since it is possible to
  // build int32 datasets that produce outputs on device when using
  // prefetch_to_device.
  // When running call ops, by default we assume that the int32 outputs are on a
  // host (except for the XLA/TPU case). So we need to special case
  // IteratorGetNext such that its eager behavior matches the wrapped one.
  // TODO(b/208435025): Remove this if we end up deciding that int32 outputs
  // from IteratorGetNext should indeed live on host.
  return op->Name() == "IteratorGetNext";
}