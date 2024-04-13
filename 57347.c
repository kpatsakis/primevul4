obj_free(mrb_state *mrb, struct RBasic *obj, int end)
{
  DEBUG(fprintf(stderr, "obj_free(%p,tt=%d)\n",obj,obj->tt));
  switch (obj->tt) {
    /* immediate - no mark */
  case MRB_TT_TRUE:
  case MRB_TT_FIXNUM:
  case MRB_TT_SYMBOL:
    /* cannot happen */
    return;

  case MRB_TT_FLOAT:
#ifdef MRB_WORD_BOXING
    break;
#else
    return;
#endif

  case MRB_TT_OBJECT:
    mrb_gc_free_iv(mrb, (struct RObject*)obj);
    break;

  case MRB_TT_EXCEPTION:
    mrb_gc_free_iv(mrb, (struct RObject*)obj);
    if ((struct RObject*)obj == mrb->backtrace.exc)
      mrb->backtrace.exc = 0;
    break;

  case MRB_TT_CLASS:
  case MRB_TT_MODULE:
  case MRB_TT_SCLASS:
    mrb_gc_free_mt(mrb, (struct RClass*)obj);
    mrb_gc_free_iv(mrb, (struct RObject*)obj);
    break;
  case MRB_TT_ICLASS:
    if (MRB_FLAG_TEST(obj, MRB_FLAG_IS_ORIGIN))
      mrb_gc_free_mt(mrb, (struct RClass*)obj);
    break;
  case MRB_TT_ENV:
    {
      struct REnv *e = (struct REnv*)obj;

      if (MRB_ENV_STACK_SHARED_P(e)) {
        /* cannot be freed */
        return;
      }
      mrb_free(mrb, e->stack);
      e->stack = NULL;
    }
    break;

  case MRB_TT_FIBER:
    {
      struct mrb_context *c = ((struct RFiber*)obj)->cxt;
      if (!end && c && c != mrb->root_c) {
        mrb_callinfo *ci = c->ci;
        mrb_callinfo *ce = c->cibase;

        while (ce <= ci) {
          struct REnv *e = ci->env;
          if (e && !is_dead(&mrb->gc, e) &&
              e->tt == MRB_TT_ENV && MRB_ENV_STACK_SHARED_P(e)) {
            mrb_env_unshare(mrb, e);
          }
          ci--;
        }
        mrb_free_context(mrb, c);
      }
    }
    break;

  case MRB_TT_ARRAY:
    if (ARY_SHARED_P(obj))
      mrb_ary_decref(mrb, ((struct RArray*)obj)->aux.shared);
    else
      mrb_free(mrb, ((struct RArray*)obj)->ptr);
    break;

  case MRB_TT_HASH:
    mrb_gc_free_iv(mrb, (struct RObject*)obj);
    mrb_gc_free_hash(mrb, (struct RHash*)obj);
    break;

  case MRB_TT_STRING:
    mrb_gc_free_str(mrb, (struct RString*)obj);
    break;

  case MRB_TT_PROC:
    {
      struct RProc *p = (struct RProc*)obj;

      if (!MRB_PROC_CFUNC_P(p) && p->body.irep) {
        mrb_irep_decref(mrb, p->body.irep);
      }
    }
    break;

  case MRB_TT_RANGE:
    mrb_free(mrb, ((struct RRange*)obj)->edges);
    break;

  case MRB_TT_DATA:
    {
      struct RData *d = (struct RData*)obj;
      if (d->type && d->type->dfree) {
        d->type->dfree(mrb, d->data);
      }
      mrb_gc_free_iv(mrb, (struct RObject*)obj);
    }
    break;

  default:
    break;
  }
  obj->tt = MRB_TT_FREE;
}
