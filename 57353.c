test_mrb_field_write_barrier(void)
{
  mrb_state *mrb = mrb_open();
  struct RBasic *obj, *value;
  mrb_gc *gc = &mrb->gc;

  puts("test_mrb_field_write_barrier");
  gc->generational = FALSE;
  obj = mrb_basic_ptr(mrb_ary_new(mrb));
  value = mrb_basic_ptr(mrb_str_new_lit(mrb, "value"));
  paint_black(obj);
  paint_partial_white(gc, value);


  puts("  in MRB_GC_STATE_MARK");
  gc->state = MRB_GC_STATE_MARK;
  mrb_field_write_barrier(mrb, obj, value);

  mrb_assert(is_gray(value));


  puts("  in MRB_GC_STATE_SWEEP");
  paint_partial_white(gc, value);
  gc->state = MRB_GC_STATE_SWEEP;
  mrb_field_write_barrier(mrb, obj, value);

  mrb_assert(obj->color & gc->current_white_part);
  mrb_assert(value->color & gc->current_white_part);


  puts("  fail with black");
  gc->state = MRB_GC_STATE_MARK;
  paint_white(obj);
  paint_partial_white(gc, value);
  mrb_field_write_barrier(mrb, obj, value);

  mrb_assert(obj->color & gc->current_white_part);


  puts("  fail with gray");
  gc->state = MRB_GC_STATE_MARK;
  paint_black(obj);
  paint_gray(value);
  mrb_field_write_barrier(mrb, obj, value);

  mrb_assert(is_gray(value));


  {
    puts("test_mrb_field_write_barrier_value");
    obj = mrb_basic_ptr(mrb_ary_new(mrb));
    mrb_value value = mrb_str_new_lit(mrb, "value");
    paint_black(obj);
    paint_partial_white(gc, mrb_basic_ptr(value));

    gc->state = MRB_GC_STATE_MARK;
    mrb_field_write_barrier_value(mrb, obj, value);

    mrb_assert(is_gray(mrb_basic_ptr(value)));
  }

  mrb_close(mrb);
}
