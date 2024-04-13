Status VerifyWrappableInCallOp(const OpDef& opdef, EagerOperation* op) {
  absl::flat_hash_set<string> opdef_attrs;
  for (const auto& attr : opdef.attr()) {
    opdef_attrs.insert(attr.name());
  }
  const auto& node_def = op->MutableAttrs()->BuildNodeDef();
  for (const auto& attr : node_def.attr()) {
    if (opdef_attrs.find(attr.first) == opdef_attrs.end()) {
      return errors::Unimplemented("EagerOperation: ", op->Name(),
                                   " has a private attr '", attr.first, "'.");
    }
  }
  return Status::OK();
}