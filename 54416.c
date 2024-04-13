static void CacheThru_put_string(HTStream *me, const char *str)
{
    if (me->status == HT_OK) {
	if (me->fp) {
	    fputs(str, me->fp);
	} else if (me->chunk) {
	    me->last_chunk = HTChunkPuts2(me->last_chunk, str);
	    if (me->last_chunk == NULL || me->last_chunk->allocated == 0)
		me->status = HT_ERROR;
	}
    }
    (*me->actions->put_string) (me->target, str);
}
