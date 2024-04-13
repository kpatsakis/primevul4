mrb_stack_extend(mrb_state *mrb, mrb_int room)
{
  if (!mrb->c->ci->stack || mrb->c->ci->stack + room >= mrb->c->stend) {
    stack_extend_alloc(mrb, room);
  }
}