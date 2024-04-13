static void CacheThru_abort(HTStream *me, HTError e)
{
    if (me->fp)
	LYCloseTempFP(me->fp);
    if (LYCacheSourceForAborted == SOURCE_CACHE_FOR_ABORTED_DROP) {
	if (me->filename) {
	    CTRACE((tfp, "SourceCacheWriter: Removing active file %s\n",
		    me->filename));
	    (void) LYRemoveTemp(me->filename);
	    FREE(me->filename);
	}
	if (me->chunk) {
	    CTRACE((tfp,
		    "SourceCacheWriter: Removing active memory chunk %p\n",
		    (void *) me->chunk));
	    HTChunkFree(me->chunk);
	}
    } else {
	me->status = HT_OK;	/*fake it */
	CacheThru_do_free(me);
    }
    (*me->actions->_abort) (me->target, e);
    FREE(me);
}
