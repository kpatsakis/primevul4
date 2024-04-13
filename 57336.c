mrb_gc_unregister(mrb_state *mrb, mrb_value obj)
{
  mrb_sym root = mrb_intern_lit(mrb, GC_ROOT_NAME);
  mrb_value table = mrb_gv_get(mrb, root);
  struct RArray *a;
  mrb_int i;

  if (mrb_nil_p(table)) return;
  if (mrb_type(table) != MRB_TT_ARRAY) {
    mrb_gv_set(mrb, root, mrb_nil_value());
    return;
  }
  a = mrb_ary_ptr(table);
  mrb_ary_modify(mrb, a);
  for (i = 0; i < a->len; i++) {
    if (mrb_obj_eq(mrb, a->ptr[i], obj)) {
      a->len--;
      memmove(&a->ptr[i], &a->ptr[i + 1], (a->len - i) * sizeof(a->ptr[i]));
      break;
    }
  }
}
