mrb_top_run(mrb_state *mrb, const struct RProc *proc, mrb_value self, mrb_int stack_keep)
{
  mrb_value v;

  if (!mrb->c->cibase) {
    return mrb_vm_run(mrb, proc, self, stack_keep);
  }
  if (mrb->c->ci == mrb->c->cibase) {
    mrb_vm_ci_env_set(mrb->c->ci, NULL);
    return mrb_vm_run(mrb, proc, self, stack_keep);
  }
  cipush(mrb, 0, CINFO_SKIP, mrb->object_class, NULL, 0, 0);
  v = mrb_vm_run(mrb, proc, self, stack_keep);

  return v;
}