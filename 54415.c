static void CacheThru_put_character(HTStream *me, int c_in)
{
    if (me->status == HT_OK) {
	if (me->fp) {
	    fputc(c_in, me->fp);
	} else if (me->chunk) {
	    me->last_chunk = HTChunkPutc2(me->last_chunk, c_in);
	    if (me->last_chunk == NULL || me->last_chunk->allocated == 0)
		me->status = HT_ERROR;
	}
    }
    (*me->actions->put_character) (me->target, c_in);
}
