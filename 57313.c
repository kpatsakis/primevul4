gc_mark_children(mrb_state *mrb, mrb_gc *gc, struct RBasic *obj)
{
  mrb_assert(is_gray(obj));
  paint_black(obj);
  gc->gray_list = obj->gcnext;
  mrb_gc_mark(mrb, (struct RBasic*)obj->c);
  switch (obj->tt) {
  case MRB_TT_ICLASS:
    {
      struct RClass *c = (struct RClass*)obj;
      if (MRB_FLAG_TEST(c, MRB_FLAG_IS_ORIGIN))
        mrb_gc_mark_mt(mrb, c);
      mrb_gc_mark(mrb, (struct RBasic*)((struct RClass*)obj)->super);
    }
    break;

  case MRB_TT_CLASS:
  case MRB_TT_MODULE:
  case MRB_TT_SCLASS:
    {
      struct RClass *c = (struct RClass*)obj;

      mrb_gc_mark_mt(mrb, c);
      mrb_gc_mark(mrb, (struct RBasic*)c->super);
    }
    /* fall through */

  case MRB_TT_OBJECT:
  case MRB_TT_DATA:
  case MRB_TT_EXCEPTION:
    mrb_gc_mark_iv(mrb, (struct RObject*)obj);
    break;

  case MRB_TT_PROC:
    {
      struct RProc *p = (struct RProc*)obj;

      mrb_gc_mark(mrb, (struct RBasic*)p->env);
      mrb_gc_mark(mrb, (struct RBasic*)p->target_class);
    }
    break;

  case MRB_TT_ENV:
    {
      struct REnv *e = (struct REnv*)obj;
      mrb_int i, len;

      if MRB_ENV_STACK_SHARED_P(e) break;
      len = MRB_ENV_STACK_LEN(e);
      for (i=0; i<len; i++) {
        mrb_gc_mark_value(mrb, e->stack[i]);
      }
    }
    break;

  case MRB_TT_FIBER:
    {
      struct mrb_context *c = ((struct RFiber*)obj)->cxt;

      if (c) mark_context(mrb, c);
    }
    break;

  case MRB_TT_ARRAY:
    {
      struct RArray *a = (struct RArray*)obj;
      size_t i, e;

      for (i=0,e=a->len; i<e; i++) {
        mrb_gc_mark_value(mrb, a->ptr[i]);
      }
    }
    break;

  case MRB_TT_HASH:
    mrb_gc_mark_iv(mrb, (struct RObject*)obj);
    mrb_gc_mark_hash(mrb, (struct RHash*)obj);
    break;

  case MRB_TT_STRING:
    break;

  case MRB_TT_RANGE:
    {
      struct RRange *r = (struct RRange*)obj;

      if (r->edges) {
        mrb_gc_mark_value(mrb, r->edges->beg);
        mrb_gc_mark_value(mrb, r->edges->end);
      }
    }
    break;

  default:
    break;
  }
}
