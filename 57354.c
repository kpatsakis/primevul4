test_mrb_write_barrier(void)
{
  mrb_state *mrb = mrb_open();
  struct RBasic *obj;
  mrb_gc *gc = &mrb->gc;

  puts("test_mrb_write_barrier");
  obj = mrb_basic_ptr(mrb_ary_new(mrb));
  paint_black(obj);

  puts("  in MRB_GC_STATE_MARK");
  gc->state = MRB_GC_STATE_MARK;
  mrb_write_barrier(mrb, obj);

  mrb_assert(is_gray(obj));
  mrb_assert(gc->atomic_gray_list == obj);


  puts("  fail with gray");
  paint_gray(obj);
  mrb_write_barrier(mrb, obj);

  mrb_assert(is_gray(obj));

  mrb_close(mrb);
}
