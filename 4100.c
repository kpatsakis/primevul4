Status StoreResourceDtypesAndShapes(const eager::Operation& remote_op,
                                    const DataTypeVector& output_dtypes,
                                    TensorHandle** retvals) {
  if (remote_op.name() == "VarHandleOp") {
    if (output_dtypes.size() != 1) {
      return errors::Internal("VarHandleOp should only have one output.");
    }
    if (output_dtypes[0] != DT_RESOURCE) {
      return errors::Internal(
          "The output of VarHandleOp should be a DT_RESOURCE.");
    }
    AttrSlice attr_slice = AttrSlice(&remote_op.attrs());
    const AttrValue* dtype;
    TF_RETURN_IF_ERROR(attr_slice.Find("dtype", &dtype));
    const AttrValue* shape;
    TF_RETURN_IF_ERROR(attr_slice.Find("shape", &shape));
    retvals[0]->SetResourceHandleDtypeAndShape(
        {DtypeAndPartialTensorShape{dtype->type(), shape->shape()}});
  }
  return Status::OK();
}