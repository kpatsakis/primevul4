HTStream *HTMLParsedPresent(HTPresentation *pres,
			    HTParentAnchor *anchor,
			    HTStream *sink)
{
    HTStream *intermediate = sink;

    if (!intermediate) {
	/*
	 * Trick to prevent HTPlainPresent from translating again.  Temporarily
	 * change UCT_STAGE_PARSER setting in anchor while the HTPlain stream
	 * is initialized, so that HTPlain sees its input and output charsets
	 * as the same.  - kw
	 */
	int old_parser_cset = HTAnchor_getUCLYhndl(anchor, UCT_STAGE_PARSER);
	int structured_cset = HTAnchor_getUCLYhndl(anchor, UCT_STAGE_STRUCTURED);

	if (structured_cset < 0)
	    structured_cset = HTAnchor_getUCLYhndl(anchor, UCT_STAGE_HTEXT);
	if (structured_cset < 0)
	    structured_cset = current_char_set;
	HTAnchor_setUCInfoStage(anchor, structured_cset,
				UCT_STAGE_PARSER, UCT_SETBY_MIME);
	if (pres->rep_out == WWW_SOURCE) {
	    /*  same effect as
	       intermediate = HTPlainPresent(pres, anchor, NULL);
	       just written in a more general way:
	     */
	    intermediate = HTStreamStack(WWW_PLAINTEXT, WWW_PRESENT,
					 NULL, anchor);
	} else {
	    /*  this too should amount to calling HTPlainPresent: */
	    intermediate = HTStreamStack(WWW_PLAINTEXT, pres->rep_out,
					 NULL, anchor);
	}
	if (old_parser_cset != structured_cset) {
	    HTAnchor_resetUCInfoStage(anchor, old_parser_cset,
				      UCT_STAGE_PARSER, UCT_SETBY_NONE);
	    if (old_parser_cset >= 0) {
		HTAnchor_setUCInfoStage(anchor, old_parser_cset,
					UCT_STAGE_PARSER,
					UCT_SETBY_DEFAULT + 1);
	    }
	}
    }
    if (!intermediate)
	return NULL;
    CTRACE((tfp, "HTMLParsedPresent calling CacheThru_new\n"));
    return CacheThru_new(anchor,
			 SGML_new(&HTML_dtd, anchor,
				  HTMLGenerator(intermediate)));
}
