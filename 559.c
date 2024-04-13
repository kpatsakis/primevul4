Node* Graph::AddNode(NodeDef node_def, Status* status) {
  const OpRegistrationData* op_reg_data;
  status->Update(ops_.LookUp(node_def.op(), &op_reg_data));
  if (!status->ok()) return nullptr;

  DataTypeVector inputs;
  DataTypeVector outputs;
  status->Update(
      InOutTypesForNode(node_def, op_reg_data->op_def, &inputs, &outputs));
  if (!status->ok()) {
    *status = AttachDef(*status, node_def);
    return nullptr;
  }

  Node::NodeClass node_class = op_reg_data->is_function_op
                                   ? Node::NC_FUNCTION_OP
                                   : Node::GetNodeClassForOp(node_def.op());

  if (op_reg_data->type_ctor != nullptr) {
    VLOG(3) << "AddNode: found type constructor for " << node_def.name();
    const auto ctor_type =
        full_type::SpecializeType(AttrSlice(node_def), op_reg_data->op_def);
    const FullTypeDef ctor_typedef = ctor_type.ValueOrDie();
    if (ctor_typedef.type_id() != TFT_UNSET) {
      *(node_def.mutable_experimental_type()) = ctor_typedef;
    }
  } else {
    VLOG(3) << "AddNode: no type constructor for " << node_def.name();
  }

  Node* node = AllocateNode(std::make_shared<NodeProperties>(
                                &op_reg_data->op_def, std::move(node_def),
                                inputs, outputs, op_reg_data->fwd_type_fn),
                            nullptr, node_class);
  return node;
}