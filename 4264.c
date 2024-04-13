mrb_protect_error(mrb_state *mrb, mrb_protect_error_func *body, void *userdata, mrb_bool *error)
{
  struct mrb_jmpbuf *prev_jmp = mrb->jmp;
  struct mrb_jmpbuf c_jmp;
  mrb_value result = mrb_nil_value();
  int ai = mrb_gc_arena_save(mrb);
  const struct mrb_context *c = mrb->c;
  ptrdiff_t ci_index = c->ci - c->cibase;

  if (error) { *error = FALSE; }

  MRB_TRY(&c_jmp) {
    mrb->jmp = &c_jmp;
    result = body(mrb, userdata);
    mrb->jmp = prev_jmp;
  }
  MRB_CATCH(&c_jmp) {
    mrb->jmp = prev_jmp;
    result = mrb_obj_value(mrb->exc);
    mrb->exc = NULL;
    if (error) { *error = TRUE; }
    if (mrb->c == c) {
      while (c->ci - c->cibase > ci_index) {
        cipop(mrb);
      }
    }
    else {
      // It was probably switched by mrb_fiber_resume().
      // Simply destroy all successive CINFO_DIRECTs once the fiber has been switched.
      c = mrb->c;
      while (c->ci > c->cibase && c->ci->cci == CINFO_DIRECT) {
        cipop(mrb);
      }
    }
  }
  MRB_END_EXC(&c_jmp);

  mrb_gc_arena_restore(mrb, ai);
  mrb_gc_protect(mrb, result);
  return result;
}