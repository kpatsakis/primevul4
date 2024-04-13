VarHandleOp::VarHandleOp(OpKernelConstruction* context) : OpKernel(context) {
  OP_REQUIRES_OK(context, context->GetAttr("container", &container_));
  OP_REQUIRES_OK(context, context->GetAttr("shared_name", &name_));

  OP_REQUIRES_OK(context, context->GetAttr("dtype", &dtype_and_shape_.dtype));
  OP_REQUIRES_OK(context, context->GetAttr("shape", &dtype_and_shape_.shape));

  is_anonymous_ = name_ == ResourceHandle::ANONYMOUS_NAME;

  if (!is_anonymous_) {
    AllocatorAttributes attr;
    attr.set_on_host(true);
    OP_REQUIRES_OK(context, context->allocate_temp(DT_RESOURCE, TensorShape({}),
                                                   &resource_, attr));
    resource_.scalar<ResourceHandle>()() = MakeResourceHandle<Var>(
        context, container_, name_,
        std::vector<DtypeAndPartialTensorShape>{dtype_and_shape_});
  }
}