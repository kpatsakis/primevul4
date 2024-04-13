static void CacheThru_write(HTStream *me, const char *str, int l)
{
    if (me->status == HT_OK && l != 0) {
	if (me->fp) {
	    if (fwrite(str, (size_t) 1, (size_t) l, me->fp) < (size_t) l
		|| ferror(me->fp)) {
		me->status = HT_ERROR;
	    }
	} else if (me->chunk) {
	    me->last_chunk = HTChunkPutb2(me->last_chunk, str, l);
	    if (me->last_chunk == NULL || me->last_chunk->allocated == 0)
		me->status = HT_ERROR;
	}
    }
    (*me->actions->put_block) (me->target, str, l);
}
