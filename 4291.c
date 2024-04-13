cipop(mrb_state *mrb)
{
  struct mrb_context *c = mrb->c;
  struct REnv *env = mrb_vm_ci_env(c->ci);

  c->ci--;
  if (env) mrb_env_unshare(mrb, env);
  return c->ci;
}