mrb_object_dead_p(mrb_state *mrb, struct RBasic *object) {
  return is_dead(&mrb->gc, object);
}
