gc_test(mrb_state *mrb, mrb_value self)
{
  test_mrb_field_write_barrier();
  test_mrb_write_barrier();
  test_add_gray_list();
  test_gc_gray_mark();
  test_incremental_gc();
  test_incremental_sweep_phase();
  return mrb_nil_value();
}
