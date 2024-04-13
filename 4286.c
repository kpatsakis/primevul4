check_method_noarg(mrb_state *mrb, const mrb_callinfo *ci)
{
  mrb_int argc = ci->n == CALL_MAXARGS ? RARRAY_LEN(ci->stack[1]) : ci->n;
  if (ci->nk > 0) {
    mrb_value kdict = ci->stack[mrb_ci_kidx(ci)];
    if (!(mrb_hash_p(kdict) && mrb_hash_empty_p(mrb, kdict))) {
      argc++;
    }
  }
  if (argc > 0) {
    mrb_argnum_error(mrb, argc, 0, 0);
  }
}