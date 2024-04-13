static HTStream *CacheThru_new(HTParentAnchor *anchor,
			       HTStream *target)
{
    char *cp_freeme = NULL;
    char filename[LY_MAXPATH];
    HTStream *stream = NULL;
    HTProtocol *p = (HTProtocol *) anchor->protocol;

    /*
     * Neatly and transparently vanish if source caching is disabled.
     */
    if (LYCacheSource == SOURCE_CACHE_NONE)
	return target;

#ifndef DEBUG_SOURCE_CACHE
    /*  Only remote HTML documents may benefit from HTreparse_document(),  */
    /*  oh, assume http protocol:                                          */
    if (strcmp(p->name, "http") != 0
	&& strcmp(p->name, "https") != 0) {
	CTRACE((tfp, "SourceCacheWriter: Protocol is \"%s\"; not cached\n", p->name));
	return target;
    }
#else
    /* all HTStreams will be cached */
#endif

    CTRACE((tfp, "start CacheThru_new\n"));

    stream = (HTStream *) malloc(sizeof(*stream));
    if (!stream)
	outofmem(__FILE__, "CacheThru_new");

    stream->isa = &PassThruCache;
    stream->anchor = anchor;
    stream->fp = NULL;
    stream->filename = NULL;
    stream->chunk = NULL;
    stream->target = target;
    stream->actions = target->isa;
    stream->status = HT_OK;

    if (LYCacheSource == SOURCE_CACHE_FILE) {

	if (anchor->source_cache_file) {
	    CTRACE((tfp,
		    "SourceCacheWriter: If successful, will replace source cache file %s\n",
		    anchor->source_cache_file));
	}

	/*
	 * We open the temp file in binary mode to make sure that
	 * end-of-line stuff and high-bit Latin-1 (or other) characters
	 * don't get munged; this way, the file should (knock on wood)
	 * contain exactly what came in from the network.
	 */
	if (!(stream->fp = LYOpenTemp(filename, HTML_SUFFIX, BIN_W))) {
	    CTRACE((tfp,
		    "SourceCacheWriter: Cannot open source cache file for URL %s\n",
		    cp_freeme = HTAnchor_address((HTAnchor *) anchor)));
	    FREE(stream);
	    FREE(cp_freeme);
	    return target;
	}

	StrAllocCopy(stream->filename, filename);

	CTRACE((tfp,
		"SourceCacheWriter: Caching source for URL %s in file %s\n",
		cp_freeme = HTAnchor_address((HTAnchor *) anchor),
		filename));
	FREE(cp_freeme);
    }

    if (LYCacheSource == SOURCE_CACHE_MEMORY) {
	if (anchor->source_cache_chunk) {
	    CTRACE((tfp,
		    "SourceCacheWriter: If successful, will replace memory chunk %p\n",
		    (void *) anchor->source_cache_chunk));
	}
	stream->chunk = stream->last_chunk = HTChunkCreateMayFail(4096, 1);
	if (!stream->chunk)	/* failed already? pretty bad... - kw */
	    stream->status = HT_ERROR;

	CTRACE((tfp,
		"SourceCacheWriter: Caching source for URL %s in memory chunk %p\n",
		cp_freeme = HTAnchor_address((HTAnchor *) anchor),
		(void *) stream->chunk));
	FREE(cp_freeme);
    }

    return stream;
}
