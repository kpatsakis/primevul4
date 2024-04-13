mrb_gc_arena_save(mrb_state *mrb)
{
  return mrb->gc.arena_idx;
}
