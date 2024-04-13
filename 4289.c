argnum_error(mrb_state *mrb, mrb_int num)
{
  mrb_value exc;
  mrb_value str;
  mrb_int argc = mrb->c->ci->n;

  if (argc == 15) {
    mrb_value args = mrb->c->ci->stack[1];
    if (mrb_array_p(args)) {
      argc = RARRAY_LEN(args);
    }
  }
  if (argc == 0 && mrb->c->ci->nk != 0 && !mrb_hash_empty_p(mrb, mrb->c->ci->stack[1])) {
    argc++;
  }
  str = mrb_format(mrb, "wrong number of arguments (given %i, expected %i)", argc, num);
  exc = mrb_exc_new_str(mrb, E_ARGUMENT_ERROR, str);
  mrb_exc_set(mrb, exc);
}