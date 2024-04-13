mrb_gc_register(mrb_state *mrb, mrb_value obj)
{
  mrb_sym root = mrb_intern_lit(mrb, GC_ROOT_NAME);
  mrb_value table = mrb_gv_get(mrb, root);

  if (mrb_nil_p(table) || mrb_type(table) != MRB_TT_ARRAY) {
    table = mrb_ary_new(mrb);
    mrb_gv_set(mrb, root, table);
  }
  mrb_ary_push(mrb, table, obj);
}
