mrb_gc_arena_restore(mrb_state *mrb, int idx)
{
  mrb_gc *gc = &mrb->gc;

#ifndef MRB_GC_FIXED_ARENA
  int capa = gc->arena_capa;

  if (idx < capa / 2) {
    capa = (int)(capa * 0.66);
    if (capa < MRB_GC_ARENA_SIZE) {
      capa = MRB_GC_ARENA_SIZE;
    }
    if (capa != gc->arena_capa) {
      gc->arena = (struct RBasic**)mrb_realloc(mrb, gc->arena, sizeof(struct RBasic*)*capa);
      gc->arena_capa = capa;
    }
  }
#endif
  gc->arena_idx = idx;
}
