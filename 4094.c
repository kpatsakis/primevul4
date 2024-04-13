Status WrapInCallOp(EagerOperation* op, EagerOperation** wrapped_op) {
  DCHECK(!op->is_function());
  const OpDef& opdef = OpRegistry::Global()->LookUp(op->Name())->op_def;
  // Raise an error for ops which don't support wrapping yet. This includes
  // ops with list inputs/outputs and ops with private attrs.
  // TODO(srbs): Support list inputs/outputs.
  TF_RETURN_IF_ERROR(VerifyWrappableInCallOp(opdef, op));

  // Build a FunctionDef containing op as a node and register with context.
  // TODO(srbs): Here we are unable to distinguish between a FunctionDef for
  // a wrapped eager op and an existing user defined function registered with
  // the context e.g. with something like
  // @tf.function
  // def __wrapped__Add(x, y):
  //   ...
  // This can be avoided by introducing a dict in EagerContext that stores a
  // mapping from the eager op's name to its unique FunctionDef name.
  auto op_attrs = op->GetOpAttrs();
  string fname;
  TF_RETURN_IF_ERROR(BuildWrappedOpName(op, opdef, op_attrs, &fname));
  if (!op->EagerContext().GetFunctionDef(fname)) {
    FunctionDef fdef;
    // Set signature.
    TF_RETURN_IF_ERROR(
        BuildWrappedOpSignature(op, opdef, fname, *fdef.mutable_signature()));
    // Add node.
    NodeDef* ndef = fdef.add_node_def();
    ndef->set_op(op->Name());
    ndef->set_name(op->Name());  // This could be anything.
    const auto& signature = fdef.signature();
    for (size_t i = 0; i < signature.input_arg_size(); i++) {
      ndef->add_input(absl::StrCat(fdef.signature().input_arg(i).name(), ":0"));
    }
    // TODO(srbs): Private attrs on the op are dropped here and applied to
    // the call op instead. If this causes problems we might have to copy those
    // attrs to this ndef. That would require updating fname to contain a hash
    // of such attributes.
    for (const auto& attr : opdef.attr()) {
      (*ndef->mutable_attr())[attr.name()].set_placeholder(attr.name());
    }
    // Set the device of this node to be the exact same one that eager mode
    // would have used.
    // TODO(b/200153278): Ideally we would just forward the call op's device at
    // runtime but currently there is no way to do it.
    ndef->set_device(op->DeviceName());

#ifdef INTEL_MKL
    if (IsMKLEnabled() &&
        absl::StartsWith(op->Name(), mkl_op_registry::kMklOpPrefix)) {
      GetMKLNodeDef(ndef);
    }
#endif  // INTEL_MKL

    // Set `ret` map.
    TF_RETURN_IF_ERROR(
        PopulateRetMap(&fdef, op_attrs, op, opdef, signature, ndef->name()));
    VLOG(1) << fdef.DebugString();
    TF_RETURN_IF_ERROR(op->EagerContext().AddFunctionDef(std::move(fdef)));
  }
  // Build the call op.
  auto& ctx = op->EagerContext();
  AbstractOperationPtr call_op(ctx.CreateOperation());
  TF_RETURN_IF_ERROR(call_op->Reset(fname.c_str(), op->DeviceName().c_str()));
  for (auto t : op->Inputs()) {
    TF_RETURN_IF_ERROR(call_op->AddInput(t));
  }
  *wrapped_op = down_cast<EagerOperation*>(call_op.release());
  // Attributes on the elementary eager operation are applied to the call op and
  // to the NodeDef inside the FunctionDef. This allows us to have a single
  // FunctionDef for different attribute values. When the function is
  // instantiated, these attributes get forwarded to the NodeDef. This is done
  // by setting the AttrValue.placeholder field for the NodeDef attrs.
  (*wrapped_op)->AddAttrs(op_attrs);
  return AddMixedTypeListAttrs(*wrapped_op, op_attrs, opdef);
}