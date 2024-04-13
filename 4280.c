mrb_run(mrb_state *mrb, const struct RProc *proc, mrb_value self)
{
  return mrb_vm_run(mrb, proc, self, mrb_ci_bidx(mrb->c->ci) + 1);
}