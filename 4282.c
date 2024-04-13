cipush(mrb_state *mrb, mrb_int push_stacks, uint8_t cci,
       struct RClass *target_class, const struct RProc *proc, mrb_sym mid, uint16_t argc)
{
  struct mrb_context *c = mrb->c;
  mrb_callinfo *ci = c->ci;

  if (ci + 1 == c->ciend) {
    ptrdiff_t size = ci - c->cibase;

    if (size > MRB_CALL_LEVEL_MAX) {
      mrb_exc_raise(mrb, mrb_obj_value(mrb->stack_err));
    }
    c->cibase = (mrb_callinfo *)mrb_realloc(mrb, c->cibase, sizeof(mrb_callinfo)*size*2);
    c->ci = c->cibase + size;
    c->ciend = c->cibase + size * 2;
  }
  ci = ++c->ci;
  ci->mid = mid;
  mrb_vm_ci_proc_set(ci, proc);
  ci->stack = ci[-1].stack + push_stacks;
  ci->n = argc & 0xf;
  ci->nk = (argc>>4) & 0xf;
  ci->cci = cci;
  ci->u.target_class = target_class;

  return ci;
}