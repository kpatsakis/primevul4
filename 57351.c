test_gc_gray_mark(void)
{
  mrb_state *mrb = mrb_open();
  mrb_value obj_v, value_v;
  struct RBasic *obj;
  size_t gray_num = 0;
  mrb_gc *gc = &mrb->gc;

  puts("test_gc_gray_mark");

  puts("  in MRB_TT_CLASS");
  obj = (struct RBasic*)mrb->object_class;
  paint_gray(obj);
  gray_num = gc_gray_mark(mrb, gc, obj);
  mrb_assert(is_black(obj));
  mrb_assert(gray_num > 1);

  puts("  in MRB_TT_ARRAY");
  obj_v = mrb_ary_new(mrb);
  value_v = mrb_str_new_lit(mrb, "test");
  paint_gray(mrb_basic_ptr(obj_v));
  paint_partial_white(gc, mrb_basic_ptr(value_v));
  mrb_ary_push(mrb, obj_v, value_v);
  gray_num = gc_gray_mark(mrb, gc, mrb_basic_ptr(obj_v));
  mrb_assert(is_black(mrb_basic_ptr(obj_v)));
  mrb_assert(is_gray(mrb_basic_ptr(value_v)));
  mrb_assert(gray_num == 1);

  mrb_close(mrb);
}
