mrb_instance_new(mrb_state *mrb, mrb_value cv)
{
  mrb_value obj, blk;
  const mrb_value *argv;
  mrb_int argc;
  mrb_sym init;

  mrb_get_args(mrb, "*!&", &argv, &argc, &blk);
  obj = mrb_instance_alloc(mrb, cv);
  init = MRB_SYM(initialize);
  if (!mrb_func_basic_p(mrb, obj, init, mrb_do_nothing)) {
    mrb_funcall_with_block(mrb, obj, init, argc, argv, blk);
  }
  return obj;
}