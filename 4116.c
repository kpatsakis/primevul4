Status BuildWrappedOpName(EagerOperation* op, const OpDef& opdef,
                          const AbstractOpAttrs* op_attrs, string* name) {
  string fname = absl::StrCat("__wrapped__", EscapeOrigName(op->Name()));
  // For every variadic arg in `args`, populates `attr_to_len` with
  // (attr_name, len(arg)).
  auto FillAttrToLen = [op_attrs, op](
                           const ProtoArgListType& args,
                           absl::btree_map<string, int>* attr_to_len) {
    for (const auto& arg : args) {
      if (!arg.type_list_attr().empty()) {
        gtl::InlinedVector<DataType, 4> type_list;
        TF_RETURN_IF_ERROR(
            op_attrs->GetTypeList(arg.type_list_attr(), &type_list));
        (*attr_to_len)[arg.type_list_attr()] = type_list.size();
      } else if (!arg.number_attr().empty()) {
        int64_t number_attr;
        if (!op_attrs->GetInt(arg.number_attr(), &number_attr)) {
          return errors::Internal("Unable to read attr ", arg.number_attr(),
                                  " for op ", op->Name());
        }
        (*attr_to_len)[arg.number_attr()] = number_attr;
      }
    }
    return Status::OK();
  };
  absl::btree_map<string, int> attr_to_len;
  TF_RETURN_IF_ERROR(FillAttrToLen(opdef.input_arg(), &attr_to_len));
  TF_RETURN_IF_ERROR(FillAttrToLen(opdef.output_arg(), &attr_to_len));
  for (auto& name_len : attr_to_len) {
    absl::StrAppend(&fname, "_", name_len.first, "_", name_len.second);
  }
  // The NodeDef in the FunctionDef gets placed on `op-DeviceName()` to ensure
  // placement consistency with eager mode.
  // TODO(b/200153278): Ideally we would just forward the call op's device at
  // runtime but currently there is no way to do it so we incur the cost of
  // creating extra FunctionDefs.
  absl::StrAppend(&fname, "_device_", op->DeviceName());
  *name = fname;
  return Status::OK();
}