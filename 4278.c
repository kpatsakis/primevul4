prepare_tagged_break(mrb_state *mrb, uint32_t tag, const struct RProc *proc, mrb_value val)
{
  if (break_tag_p((struct RBreak*)mrb->exc, tag)) {
    mrb_break_tag_set((struct RBreak*)mrb->exc, tag);
  }
  else {
    mrb->exc = (struct RObject*)break_new(mrb, tag, proc, val);
  }
}