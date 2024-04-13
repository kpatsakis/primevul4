mrb_env_unshare(mrb_state *mrb, struct REnv *e)
{
  if (e == NULL) return;
  else {
    size_t len = (size_t)MRB_ENV_LEN(e);
    mrb_value *p;

    if (!MRB_ENV_ONSTACK_P(e)) return;
    if (e->cxt != mrb->c) return;
    if (e == mrb_vm_ci_env(mrb->c->cibase)) return; /* for mirb */
    p = (mrb_value *)mrb_malloc(mrb, sizeof(mrb_value)*len);
    if (len > 0) {
      stack_copy(p, e->stack, len);
    }
    e->stack = p;
    MRB_ENV_CLOSE(e);
    mrb_write_barrier(mrb, (struct RBasic *)e);
  }
}