check_target_class(mrb_state *mrb)
{
  struct RClass *target = mrb_vm_ci_target_class(mrb->c->ci);
  if (!target) {
    mrb_value exc = mrb_exc_new_lit(mrb, E_TYPE_ERROR, "no target class or module");
    mrb_exc_set(mrb, exc);
  }
  return target;
}