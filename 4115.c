Status MustCompileWithXLA(const EagerOperation* op, const EagerContext& ctx,
                          bool* compile_with_xla) {
  if (!op->is_function()) {
    *compile_with_xla = false;
    return Status::OK();
  }

  if (op->eager_func_params().has_value() &&
      op->eager_func_params().value().step_id.has_value()) {
    // If the op is a component of a multi-device function, don't compile it
    // with XLA.
    *compile_with_xla = false;
    return Status::OK();
  }

  Status status = GetFuncAttr(op, ctx, kXlaMustCompileAttr, compile_with_xla);
  if (status.ok()) {
    return Status::OK();
  }

  // No explicit requests. Compile for XLA devices by default.
  if (op->GetDeviceParsedName().type == "TPU" ||
      op->GetDeviceParsedName().type == "XLA_GPU" ||
      op->GetDeviceParsedName().type == "XLA_CPU") {
    VLOG(2) << "Compiling " << op->Name()
            << " with XLA because it is running on an XLA device "
            << op->GetDeviceParsedName().type;
    *compile_with_xla = true;
  } else {
    *compile_with_xla = false;
  }

  return Status::OK();
}