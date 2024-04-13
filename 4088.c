Status ValidateOp(EagerOperation* op) {
  const NodeDef& node_def = op->MutableAttrs()->BuildNodeDef();
  const OpDef* op_def;
  TF_RETURN_IF_ERROR(OpRegistry::Global()->LookUpOpDef(node_def.op(), &op_def));
  return ValidateNodeDef(node_def, *op_def);
}