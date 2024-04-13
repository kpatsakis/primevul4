void HTML_put_character(HTStructured * me, int c)
{
    unsigned uc = UCH(c);

    /*
     * Ignore all non-MAP content when just scanning a document for MAPs.  - FM
     */
    if (LYMapsOnly && me->sp[0].tag_number != HTML_OBJECT)
	return;

    c = (int) uc;

    /*
     * Do EOL conversion if needed.  - FM
     *
     * Convert EOL styles:
     *   macintosh:  cr    --> lf
     *   ascii:      cr-lf --> lf
     *   unix:       lf    --> lf
     */
    if ((me->lastraw == '\r') && c == '\n') {
	me->lastraw = -1;
	return;
    }
    me->lastraw = c;
    if (c == '\r') {
	c = '\n';
	uc = UCH(c);
    }

    /*
     * Handle SGML_LITTERAL tags that have HTChunk elements.  - FM
     */
    switch (me->sp[0].tag_number) {

    case HTML_COMMENT:
	return;			/* Do Nothing */

    case HTML_TITLE:
	if (c == LY_SOFT_HYPHEN)
	    return;
	if (c != '\n' && c != '\t' && c != '\r') {
	    HTChunkPutc(&me->title, uc);
	} else if (FIX_JAPANESE_SPACES) {
	    if (c == '\t') {
		HTChunkPutc(&me->title, ' ');
	    } else {
		return;
	    }
	} else {
	    HTChunkPutc(&me->title, ' ');
	}
	return;

    case HTML_STYLE:
	HTChunkPutc(&me->style_block, uc);
	return;

    case HTML_SCRIPT:
	HTChunkPutc(&me->script, uc);
	return;

    case HTML_OBJECT:
	HTChunkPutc(&me->object, uc);
	return;

    case HTML_TEXTAREA:
	HTChunkPutc(&me->textarea, uc);
	return;

    case HTML_SELECT:
    case HTML_OPTION:
	HTChunkPutc(&me->option, uc);
	return;

    case HTML_MATH:
	HTChunkPutc(&me->math, uc);
	return;

    default:
	if (me->inSELECT) {
	    /*
	     * If we are within a SELECT not caught by the cases above -
	     * HTML_SELECT or HTML_OPTION may not be the last element pushed on
	     * the style stack if there were invalid markup tags within a
	     * SELECT element.  For error recovery, treat text as part of the
	     * OPTION text, it is probably meant to show up as user-visible
	     * text.  Having A as an open element while in SELECT is really
	     * sick, don't make anchor text part of the option text in that
	     * case since the option text will probably just be discarded.  -
	     * kw
	     */
	    if (me->sp[0].tag_number == HTML_A)
		break;
	    HTChunkPutc(&me->option, uc);
	    return;
	}
	break;
    }				/* end first switch */

    /*
     * Handle all other tag content.  - FM
     */
    switch (me->sp[0].tag_number) {

    case HTML_PRE:		/* Formatted text */
	/*
	 * We guarantee that the style is up-to-date in begin_litteral. But we
	 * still want to strip \r's.
	 */
	if (c != '\r' &&
	    !(c == '\n' && me->inLABEL && !me->inP) &&
	    !(c == '\n' && !me->inPRE)) {
	    me->inP = TRUE;
	    me->inLABEL = FALSE;
	    HText_appendCharacter(me->text, c);
	}
	me->inPRE = TRUE;
	break;

    case HTML_LISTING:		/* Literal text */
    case HTML_XMP:
    case HTML_PLAINTEXT:
	/*
	 * We guarantee that the style is up-to-date in begin_litteral.  But we
	 * still want to strip \r's.
	 */
	if (c != '\r') {
	    me->inP = TRUE;
	    me->inLABEL = FALSE;
	    HText_appendCharacter(me->text, c);
	}
	break;

    default:
	/*
	 * Free format text.
	 */
	if (me->sp->style->id == ST_Preformatted) {
	    if (c != '\r' &&
		!(c == '\n' && me->inLABEL && !me->inP) &&
		!(c == '\n' && !me->inPRE)) {
		me->inP = TRUE;
		me->inLABEL = FALSE;
		HText_appendCharacter(me->text, c);
	    }
	    me->inPRE = TRUE;

	} else if (me->sp->style->id == ST_Listing ||
		   me->sp->style->id == ST_Example) {
	    if (c != '\r') {
		me->inP = TRUE;
		me->inLABEL = FALSE;
		HText_appendCharacter(me->text, c);
	    }

	} else {
	    if (me->style_change) {
		if ((c == '\n') || (c == ' '))
		    return;	/* Ignore it */
		UPDATE_STYLE;
	    }
	    if (c == '\n') {
		if (!FIX_JAPANESE_SPACES) {
		    if (me->in_word) {
			if (HText_getLastChar(me->text) != ' ') {
			    me->inP = TRUE;
			    me->inLABEL = FALSE;
			    HText_appendCharacter(me->text, ' ');
			}
			me->in_word = NO;
		    }
		}

	    } else if (c == ' ' || c == '\t') {
		if (HText_getLastChar(me->text) != ' ') {
		    me->inP = TRUE;
		    me->inLABEL = FALSE;
		    HText_appendCharacter(me->text, ' ');
		}

	    } else if (c == '\r') {
		/* ignore */

	    } else {
		me->inP = TRUE;
		me->inLABEL = FALSE;
		HText_appendCharacter(me->text, c);
		me->in_word = YES;
	    }
	}
    }				/* end second switch */

    if (c == '\n' || c == '\t') {
	HText_setLastChar(me->text, ' ');	/* set it to a generic separator */
    } else {
	HText_setLastChar(me->text, c);
    }
}
