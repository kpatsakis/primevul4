Status PopulateRetMap(FunctionDef* fdef, const AbstractOpAttrs* op_attrs,
                      const EagerOperation* op, const OpDef& opdef,
                      const OpDef& signature, const string& node_name) {
  int next_sig_output = 0;
  for (size_t i = 0; i < opdef.output_arg_size(); i++) {
    const auto& output_arg = opdef.output_arg(i);
    if (!output_arg.type_list_attr().empty()) {
      gtl::InlinedVector<DataType, 4> type_list;
      TF_RETURN_IF_ERROR(
          op_attrs->GetTypeList(output_arg.type_list_attr(), &type_list));
      for (int j = 0; j < type_list.size(); j++) {
        (*fdef->mutable_ret())[signature.output_arg(next_sig_output++).name()] =
            absl::StrCat(node_name, ":", output_arg.name(), ":", j);
      }
    } else if (!output_arg.number_attr().empty()) {
      int64_t number_attr;
      if (!op_attrs->GetInt(output_arg.number_attr(), &number_attr)) {
        return errors::Internal("Unable to read attr ",
                                output_arg.number_attr(), " for op ",
                                op->Name());
      }
      for (int j = 0; j < number_attr; j++) {
        (*fdef->mutable_ret())[signature.output_arg(next_sig_output++).name()] =
            absl::StrCat(node_name, ":", output_arg.name(), ":", j);
      }
    } else {
      (*fdef->mutable_ret())[signature.output_arg(next_sig_output++).name()] =
          absl::StrCat(node_name, ":", output_arg.name(), ":0");
    }
  }
  return Status::OK();
}