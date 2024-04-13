mrb_funcall_with_block(mrb_state *mrb, mrb_value self, mrb_sym mid, mrb_int argc, const mrb_value *argv, mrb_value blk)
{
  mrb_value val;
  int ai = mrb_gc_arena_save(mrb);

  if (!mrb->jmp) {
    struct mrb_jmpbuf c_jmp;
    ptrdiff_t nth_ci = mrb->c->ci - mrb->c->cibase;

    MRB_TRY(&c_jmp) {
      mrb->jmp = &c_jmp;
      /* recursive call */
      val = mrb_funcall_with_block(mrb, self, mid, argc, argv, blk);
      mrb->jmp = 0;
    }
    MRB_CATCH(&c_jmp) { /* error */
      while (nth_ci < (mrb->c->ci - mrb->c->cibase)) {
        cipop(mrb);
      }
      mrb->jmp = 0;
      val = mrb_obj_value(mrb->exc);
    }
    MRB_END_EXC(&c_jmp);
    mrb->jmp = 0;
  }
  else {
    mrb_method_t m;
    struct RClass *c;
    mrb_callinfo *ci = mrb->c->ci;
    mrb_int n = mrb_ci_nregs(ci);
    ptrdiff_t voff = -1;

    if (!mrb->c->stbase) {
      stack_init(mrb);
    }
    if (ci - mrb->c->cibase > MRB_CALL_LEVEL_MAX) {
      mrb_exc_raise(mrb, mrb_obj_value(mrb->stack_err));
    }
    if (mrb->c->stbase <= argv && argv < mrb->c->stend) {
      voff = argv - mrb->c->stbase;
    }
    if (argc < 0) {
      mrb_raisef(mrb, E_ARGUMENT_ERROR, "negative argc for funcall (%i)", argc);
    }
    c = mrb_class(mrb, self);
    m = mrb_method_search_vm(mrb, &c, mid);
    mrb_stack_extend(mrb, n + argc + 3);
    if (argc >= 15) {
      ci->stack[n+1] = mrb_ary_new_from_values(mrb, argc, argv);
      argc = 15;
    }
    if (MRB_METHOD_UNDEF_P(m)) {
      uint16_t ac = (uint16_t)argc;
      m = prepare_missing(mrb, self, mid, &c, (uint32_t)n, &ac, mrb_nil_value(), 0);
      argc = (mrb_int)ac;
      mid = MRB_SYM(method_missing);
    }
    ci = cipush(mrb, n, 0, c, NULL, mid, (uint16_t)argc);
    if (MRB_METHOD_PROC_P(m)) {
      struct RProc *p = MRB_METHOD_PROC(m);

      mrb_vm_ci_proc_set(ci, p);
      if (!MRB_PROC_CFUNC_P(p)) {
        mrb_stack_extend(mrb, p->body.irep->nregs + argc);
      }
    }
    if (voff >= 0) {
      argv = mrb->c->stbase + voff;
    }
    ci->stack[0] = self;
    if (argc < 15) {
      if (argc > 0)
        stack_copy(ci->stack+1, argv, argc);
      ci->stack[argc+1] = blk;
    }
    else {
      ci->stack[2] = blk;
    }

    if (MRB_METHOD_CFUNC_P(m)) {
      ci->cci = CINFO_DIRECT;
      val = MRB_METHOD_CFUNC(m)(mrb, self);
      cipop(mrb);
    }
    else {
      ci->cci = CINFO_SKIP;
      val = mrb_run(mrb, MRB_METHOD_PROC(m), self);
    }
  }
  mrb_gc_arena_restore(mrb, ai);
  mrb_gc_protect(mrb, val);
  return val;
}