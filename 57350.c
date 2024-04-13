test_add_gray_list(void)
{
  mrb_state *mrb = mrb_open();
  struct RBasic *obj1, *obj2;
  mrb_gc *gc = &mrb->gc;

  puts("test_add_gray_list");
  change_gen_gc_mode(mrb, gc, FALSE);
  mrb_assert(gc->gray_list == NULL);
  obj1 = mrb_basic_ptr(mrb_str_new_lit(mrb, "test"));
  add_gray_list(mrb, gc, obj1);
  mrb_assert(gc->gray_list == obj1);
  mrb_assert(is_gray(obj1));

  obj2 = mrb_basic_ptr(mrb_str_new_lit(mrb, "test"));
  add_gray_list(mrb, gc, obj2);
  mrb_assert(gc->gray_list == obj2);
  mrb_assert(gc->gray_list->gcnext == obj1);
  mrb_assert(is_gray(obj2));

  mrb_close(mrb);
}
