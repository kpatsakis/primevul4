static void CacheThru_free(HTStream *me)
{
    CacheThru_do_free(me);
    (*me->actions->_free) (me->target);
    FREE(me);
}
