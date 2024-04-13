Status LocalEagerCopyToDevice(TensorHandle* h, EagerContext* ctx,
                              EagerExecutor* executor, Device* dstd,
                              bool mirror, TensorHandle** result) {
  TF_RETURN_IF_ERROR(executor->status());
  Device* d = ctx->CanonicalDevice(dstd);
  if (mirror && h->HasLocalMirror(d)) {
    h->Ref();
    *result = h;
    return Status::OK();
  }

  bool async = executor->Async();
  if (mirror) {
    h->Ref();
    *result = h;

    if (h->HasLocalMirror(d)) {
      return Status::OK();
    }

    // We don't bother adding an empty local mirror in sync mode since we'll be
    // executing the operation directly and be calling AddLocalMirror. A
    // reference count is still needed which will be removed if the operation
    // fails.
    if (async) {
      Status s = h->AddEmptyLocalMirror(d);
      if (!s.ok()) {
        // If a mirror was added since we called HasLocalMirror then just return
        // since another thread has already added the mirror.
        if (s.code() == error::Code::ALREADY_EXISTS) {
          return Status::OK();
        }

        // Remove the previously added reference count since adding the mirror
        // failed.
        h->Unref();
        *result = nullptr;
        return s;
      }
    }
  } else {
    *result = TensorHandle::CreateEmptyLocalHandle(
        d, dstd, h->resource_device(), h->dtype, ctx);
  }

  Status s;
  if (async) {
    // Note that `h` may not be currently ready. However execution order will
    // make sure that `h` is ready before the copy is actually done.
    std::unique_ptr<EagerNode> node(
        new CopyToDeviceNode(h, *result, d, *ctx, async, mirror));
    s = executor->AddOrExecute(std::move(node));
  } else {
    CopyToDeviceNode node(h, *result, d, *ctx, async, mirror);
    s = executor->SyncExecute(&node);
  }

  // Since the operation failed, we need to Unref any outputs that were
  // allocated.
  if (!s.ok()) {
    (*result)->Unref();
    *result = nullptr;
  }

  return s;
}