Status AddMixedTypeListAttrs(EagerOperation* wrapped_op,
                             const AbstractOpAttrs* op_attrs,
                             const OpDef& opdef) {
  auto FillAttrsToAdd =
      [op_attrs](const ProtoArgListType& opdef_args,
                 absl::flat_hash_map<string, DataType>* attrs_to_add) {
        for (const auto& arg : opdef_args) {
          if (!arg.type_list_attr().empty()) {
            gtl::InlinedVector<DataType, 4> type_list;
            TF_RETURN_IF_ERROR(
                op_attrs->GetTypeList(arg.type_list_attr(), &type_list));
            for (size_t i = 0; i < type_list.size(); i++) {
              auto attr_name = GetFlatName(arg.type_list_attr(), i);
              (*attrs_to_add)[attr_name] = type_list[i];
            }
          }
        }
        return Status::OK();
      };
  absl::flat_hash_map<string, DataType> attrs_to_add;
  TF_RETURN_IF_ERROR(FillAttrsToAdd(opdef.input_arg(), &attrs_to_add));
  TF_RETURN_IF_ERROR(FillAttrsToAdd(opdef.output_arg(), &attrs_to_add));
  for (auto& name_type : attrs_to_add) {
    TF_RETURN_IF_ERROR(
        wrapped_op->SetAttrType(name_type.first.data(), name_type.second));
  }
  // TODO(srbs): Rename all original attributes using EscapeOrigName.
  return Status::OK();
}