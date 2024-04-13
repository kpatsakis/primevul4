prepare_missing(mrb_state *mrb, mrb_value recv, mrb_sym mid, struct RClass **clsp, uint32_t a, uint16_t *c, mrb_value blk, int super)
{
  mrb_sym missing = MRB_SYM(method_missing);
  mrb_callinfo *ci = mrb->c->ci;
  uint16_t b = *c;
  mrb_int n = b & 0xf;
  mrb_int nk = (b>>4) & 0xf;
  mrb_value *argv = &ci->stack[a+1];
  mrb_value args;
  mrb_method_t m;

  /* pack positional arguments */
  if (n == 15) args = argv[0];
  else args = mrb_ary_new_from_values(mrb, n, argv);

  if (mrb_func_basic_p(mrb, recv, missing, mrb_obj_missing)) {
  method_missing:
    if (super) mrb_no_method_error(mrb, mid, args, "no superclass method '%n'", mid);
    else mrb_method_missing(mrb, mid, recv, args);
    /* not reached */
  }
  if (mid != missing) {
    *clsp = mrb_class(mrb, recv);
  }
  m = mrb_method_search_vm(mrb, clsp, missing);
  if (MRB_METHOD_UNDEF_P(m)) goto method_missing; /* just in case */
  mrb_stack_extend(mrb, a+4);

  argv = &ci->stack[a+1];       /* maybe reallocated */
  argv[0] = args;
  if (nk == 0) {
    argv[1] = blk;
  }
  else {
    mrb_assert(nk == 15);
    argv[1] = argv[n];
    argv[2] = blk;
  }
  *c = 15 | (uint16_t)(nk<<4);
  mrb_ary_unshift(mrb, args, mrb_symbol_value(mid));
  return m;
}