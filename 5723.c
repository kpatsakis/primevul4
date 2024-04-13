boot_defclass(mrb_state *mrb, struct RClass *super)
{
  struct RClass *c;

  c = MRB_OBJ_ALLOC(mrb, MRB_TT_CLASS, mrb->class_class);
  if (super) {
    c->super = super;
    mrb_field_write_barrier(mrb, (struct RBasic*)c, (struct RBasic*)super);
    c->flags |= MRB_FL_CLASS_IS_INHERITED;
  }
  else {
    c->super = mrb->object_class;
  }
  c->mt = mt_new(mrb);
  return c;
}