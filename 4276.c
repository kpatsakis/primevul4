break_new(mrb_state *mrb, uint32_t tag, const struct RProc *p, mrb_value val)
{
  struct RBreak *brk;

  brk = MRB_OBJ_ALLOC(mrb, MRB_TT_BREAK, NULL);
  mrb_break_proc_set(brk, p);
  mrb_break_value_set(brk, val);
  mrb_break_tag_set(brk, tag);

  return brk;
}