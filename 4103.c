Status GetFuncAttr(const EagerOperation* op, const EagerContext& ctx,
                   const char* attr_name, bool* value) {
  Status status = op->Attrs().Get(attr_name, value);
  if (status.ok()) {
    VLOG(2) << "Caller explicitly specifies "
            << (attr_name ? "=true " : "=false, ") << op->DebugString();
    return Status::OK();
  }

  const FunctionDef* function_def =
      ctx.pflr()->GetFunctionLibraryDefinition()->Find(op->Name());
  if (function_def == nullptr) {
    return errors::NotFound("Failed to find function '", op->Name(), "'");
  }

  status = GetNodeAttr(AttrSlice(&function_def->attr()), attr_name, value);
  if (status.ok()) {
    VLOG(2) << "Function definition explicitly specifies "
            << (attr_name ? "=true" : "=false");
    return Status::OK();
  }
  return status;
}