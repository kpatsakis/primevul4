Status BuildWrappedOpSignature(EagerOperation* op, const OpDef& opdef,
                               const string& fname, OpDef& signature) {
  signature = opdef;
  signature.clear_input_arg();
  signature.clear_output_arg();
  signature.set_name(fname);
  auto op_attrs = op->GetOpAttrs();
  auto FillSignatureArgs = [op_attrs, op](
                               const ProtoArgListType& opdef_args,
                               ProtoArgListType* sig_args,
                               absl::flat_hash_set<string>& new_attrs) {
    for (const auto& arg : opdef_args) {
      if (!arg.type_list_attr().empty()) {
        gtl::InlinedVector<DataType, 4> type_list;
        TF_RETURN_IF_ERROR(
            op_attrs->GetTypeList(arg.type_list_attr(), &type_list));
        for (size_t i = 0; i < type_list.size(); i++) {
          auto arg_def = sig_args->Add();
          arg_def->set_name(GetFlatName(arg.name(), i));
          auto attr_name = GetFlatName(arg.type_list_attr(), i);
          new_attrs.insert(attr_name);
          arg_def->set_type_attr(std::move(attr_name));
        }
      } else if (!arg.number_attr().empty()) {
        int64_t number_attr;
        if (!op_attrs->GetInt(arg.number_attr(), &number_attr)) {
          return errors::Internal("Unable to read attr ", arg.number_attr(),
                                  " for op ", op->Name());
        }
        for (int64_t i = 0; i < number_attr; i++) {
          auto arg_def = sig_args->Add();
          arg_def->set_name(GetFlatName(arg.name(), i));
          if (!arg.type_attr().empty()) {
            arg_def->set_type_attr(arg.type_attr());
          } else {
            arg_def->set_type(arg.type());
          }
        }
      } else {
        auto arg_def = sig_args->Add();
        *arg_def = arg;
        arg_def->set_name(EscapeOrigName(arg.name()));
        if (!arg.type_attr().empty()) {
          // Don't escape: type attrs are still referenced by the original name.
          arg_def->set_type_attr(arg.type_attr());
        }
      }
    }
    return Status::OK();
  };
  absl::flat_hash_set<string> new_attrs;
  TF_RETURN_IF_ERROR(FillSignatureArgs(
      opdef.input_arg(), signature.mutable_input_arg(), new_attrs));
  TF_RETURN_IF_ERROR(FillSignatureArgs(
      opdef.output_arg(), signature.mutable_output_arg(), new_attrs));
  for (auto& attr_name : new_attrs) {
    auto attr_def = signature.mutable_attr()->Add();
    attr_def->set_name(attr_name);
    attr_def->set_type("type");
  }
  return Status::OK();
}