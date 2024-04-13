mrb_ci_kidx(const mrb_callinfo *ci)
{
  return (ci->n == CALL_MAXARGS) ? 2 : ci->n + 1;
}