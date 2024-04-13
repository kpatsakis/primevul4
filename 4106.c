void PrepareRemoteOp(eager::Operation* remote_op, EagerOperation* op) {
  EagerContext& ctx = op->EagerContext();

  remote_op->set_id(ctx.RemoteMgr()->NextOpId());
  remote_op->set_name(op->Name());

  op->Attrs().FillAttrValueMapWithoutDefaults(remote_op->mutable_attrs());
  remote_op->set_device(absl::get<Device*>(op->Device())->name());
  remote_op->set_is_function(op->is_function());
}