catch_handler_find(mrb_state *mrb, mrb_callinfo *ci, const mrb_code *pc, uint32_t filter)
{
  const mrb_irep *irep;
  ptrdiff_t xpc;
  size_t cnt;
  const struct mrb_irep_catch_handler *e;

/* The comparison operators use `>` and `<=` because pc already points to the next instruction */
#define catch_cover_p(pc, beg, end) ((pc) > (ptrdiff_t)(beg) && (pc) <= (ptrdiff_t)(end))

  if (ci->proc == NULL || MRB_PROC_CFUNC_P(ci->proc)) return NULL;
  irep = ci->proc->body.irep;
  if (irep->clen < 1) return NULL;
  xpc = pc - irep->iseq;
  /* If it retry at the top level, pc will be 0, so check with -1 as the start position */
  mrb_assert(catch_cover_p(xpc, -1, irep->ilen));
  if (!catch_cover_p(xpc, -1, irep->ilen)) return NULL;

  /* Currently uses a simple linear search to avoid processing complexity. */
  cnt = irep->clen;
  e = mrb_irep_catch_handler_table(irep) + cnt - 1;
  for (; cnt > 0; cnt --, e --) {
    if (((UINT32_C(1) << e->type) & filter) &&
        catch_cover_p(xpc, mrb_irep_catch_handler_unpack(e->begin), mrb_irep_catch_handler_unpack(e->end))) {
      return e;
    }
  }

#undef catch_cover_p

  return NULL;
}