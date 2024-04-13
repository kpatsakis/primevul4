static void CacheThru_do_free(HTStream *me)
{
    if (me->anchor->source_cache_file) {
	CTRACE((tfp, "SourceCacheWriter: Removing previous file %s\n",
		me->anchor->source_cache_file));
	(void) LYRemoveTemp(me->anchor->source_cache_file);
	FREE(me->anchor->source_cache_file);
    }
    if (me->anchor->source_cache_chunk) {
	CTRACE((tfp, "SourceCacheWriter: Removing previous memory chunk %p\n",
		(void *) me->anchor->source_cache_chunk));
	HTChunkFree(me->anchor->source_cache_chunk);
	me->anchor->source_cache_chunk = NULL;
    }
    if (me->fp) {
	fflush(me->fp);
	if (ferror(me->fp))
	    me->status = HT_ERROR;
	LYCloseTempFP(me->fp);
	if (me->status == HT_OK) {
	    char *cp_freeme = 0;

	    me->anchor->source_cache_file = me->filename;
	    CTRACE((tfp,
		    "SourceCacheWriter: Committing file %s for URL %s to anchor\n",
		    me->filename,
		    cp_freeme = HTAnchor_address((HTAnchor *) me->anchor)));
	    FREE(cp_freeme);
	} else {
	    if (source_cache_file_error == FALSE) {
		HTAlert(gettext("Source cache error - disk full?"));
		source_cache_file_error = TRUE;
	    }
	    (void) LYRemoveTemp(me->filename);
	    me->anchor->source_cache_file = NULL;
	}
    } else if (me->status != HT_OK) {
	if (me->chunk) {
	    CTRACE((tfp, "SourceCacheWriter: memory chunk %p had errors.\n",
		    (void *) me->chunk));
	    HTChunkFree(me->chunk);
	    me->chunk = me->last_chunk = NULL;
	}
	HTAlert(gettext("Source cache error - not enough memory!"));
    }
    if (me->chunk) {
	char *cp_freeme = NULL;

	me->anchor->source_cache_chunk = me->chunk;
	CTRACE((tfp,
		"SourceCacheWriter: Committing memory chunk %p for URL %s to anchor\n",
		(void *) me->chunk,
		cp_freeme = HTAnchor_address((HTAnchor *) me->anchor)));
	FREE(cp_freeme);
    }
}
