mrb_free(mrb_state *mrb, void *p)
{
  (mrb->allocf)(mrb, p, 0, mrb->allocf_ud);
}
