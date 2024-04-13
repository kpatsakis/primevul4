Status GetBuffer(OpKernelContext* ctx, const NodeDef& ndef, Buffer** buf) {
  auto rm = ctx->resource_manager();
  ContainerInfo cinfo;

  // Lambda for creating the Staging Area
  auto create_fn = [&ndef](Buffer** ret) -> Status {
    int64_t capacity;
    int64_t memory_limit;
    TF_RETURN_IF_ERROR(GetNodeAttr(ndef, "capacity", &capacity));
    TF_RETURN_IF_ERROR(GetNodeAttr(ndef, "memory_limit", &memory_limit));
    *ret = new Buffer(capacity, memory_limit);
    return Status::OK();
  };

  TF_RETURN_IF_ERROR(cinfo.Init(rm, ndef, true /* use name() */));
  TF_RETURN_IF_ERROR(rm->LookupOrCreate<Buffer>(cinfo.container(), cinfo.name(),
                                                buf, create_fn));
  return Status::OK();
}