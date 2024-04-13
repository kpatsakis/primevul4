static void HTML_free(HTStructured * me)
{
    char *include = NULL;

    if (LYMapsOnly && !me->text) {
	/*
	 * We only handled MAP, AREA and BASE tags, and didn't create an HText
	 * structure for the document nor want one now, so just make sure we
	 * free anything that might have been allocated.  - FM
	 */
	FREE(me->base_href);
	FREE(me->map_address);
	clear_objectdata(me);
	FREE(me->xinclude);
	FREE(me);
	return;
    }

    UPDATE_STYLE;		/* Creates empty document here! */
    if (me->comment_end)
	HTML_put_string(me, me->comment_end);
    if (me->text) {
	/*
	 * Emphasis containers, A, FONT, and FORM may be declared SGML_EMPTY in
	 * HTMLDTD.c, and SGML_character() in SGML.c may check for their end
	 * tags to call HTML_end_element() directly (with a check in that to
	 * bypass decrementing of the HTML parser's stack).  So if we still
	 * have the emphasis (Underline) on, or any open A, FONT, or FORM
	 * containers, turn it off or close them now.  - FM & kw
	 *
	 * IF those tags are not declared SGML_EMPTY, but we let the SGML.c
	 * parser take care of correctly stacked ordering, and of correct
	 * wind-down on end-of-stream (in SGML_free SGML_abort), THEN these and
	 * other checks here in HTML.c should not be necessary.  Still it can't
	 * hurt to include them.  - kw
	 */
	if (me->inUnderline) {
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	    me->inUnderline = FALSE;
	    me->Underline_Level = 0;
	    CTRACE((tfp, "HTML_free: Ending underline\n"));
	}
	if (me->inA) {
	    HTML_end_element(me, HTML_A, &include);
	    me->inA = FALSE;
	    CTRACE((tfp, "HTML_free: Ending HTML_A\n"));
	}
	if (me->inFONT) {
	    HTML_end_element(me, HTML_FONT, &include);
	    me->inFONT = FALSE;
	}
	if (me->inFORM) {
	    HTML_end_element(me, HTML_FORM, &include);
	    me->inFORM = FALSE;
	}
	if (me->option.size > 0) {
	    /*
	     * If we still have data in the me->option chunk after forcing a
	     * close of a still-open form, something must have gone very wrong. 
	     * - kw
	     */
	    if (LYBadHTML(me)) {
		LYShowBadHTML("Bad HTML: SELECT or OPTION not ended properly *****\n");
	    }
	    HTChunkTerminate(&me->option);
	    /*
	     * Output the left-over data as text, maybe it was invalid markup
	     * meant to be shown somewhere.  - kw
	     */
	    CTRACE((tfp, "HTML_free: ***** leftover option data: %s\n",
		    me->option.data));
	    HTML_put_string(me, me->option.data);
	    HTChunkClear(&me->option);
	}
	if (me->textarea.size > 0) {
	    /*
	     * If we still have data in the me->textarea chunk after forcing a
	     * close of a still-open form, something must have gone very wrong. 
	     * - kw
	     */
	    if (LYBadHTML(me)) {
		LYShowBadHTML("Bad HTML: TEXTAREA not used properly *****\n");
	    }
	    HTChunkTerminate(&me->textarea);
	    /*
	     * Output the left-over data as text, maybe it was invalid markup
	     * meant to be shown somewhere.  - kw
	     */
	    CTRACE((tfp, "HTML_free: ***** leftover textarea data: %s\n",
		    me->textarea.data));
	    HTML_put_string(me, me->textarea.data);
	    HTChunkClear(&me->textarea);
	}
	/*
	 * If we're interactive and have hidden links but no visible links, add
	 * a message informing the user about this and suggesting use of the
	 * 'l'ist command.  - FM
	 */
	if (!dump_output_immediately &&
	    HText_sourceAnchors(me->text) < 1 &&
	    HText_HiddenLinkCount(me->text) > 0) {
	    HTML_start_element(me, HTML_P, 0, 0, -1, &include);
	    HTML_put_character(me, '[');
	    HTML_start_element(me, HTML_EM, 0, 0, -1, &include);
	    HTML_put_string(me,
			    gettext("Document has only hidden links.  Use the 'l'ist command."));
	    HTML_end_element(me, HTML_EM, &include);
	    HTML_put_character(me, ']');
	    HTML_end_element(me, HTML_P, &include);
	}
	if (me->xinclude) {
	    HText_appendText(me->text, " *** LYNX ERROR ***\rUnparsed data:\r");
	    HText_appendText(me->text, me->xinclude);
	    FREE(me->xinclude);
	}

	/*
	 * Now call the cleanup function.  - FM
	 */
	HText_endAppend(me->text);
    }
    if (me->option.size > 0) {
	/*
	 * If we still have data in the me->option chunk after forcing a close
	 * of a still-open form, something must have gone very wrong.  - kw
	 */
	if (LYBadHTML(me)) {
	    LYShowBadHTML("Bad HTML: SELECT or OPTION not ended properly *****\n");
	}
	if (TRACE) {
	    HTChunkTerminate(&me->option);
	    CTRACE((tfp, "HTML_free: ***** leftover option data: %s\n",
		    me->option.data));
	}
	HTChunkClear(&me->option);
    }
    if (me->textarea.size > 0) {
	/*
	 * If we still have data in the me->textarea chunk after forcing a
	 * close of a still-open form, something must have gone very wrong.  -
	 * kw
	 */
	if (LYBadHTML(me)) {
	    LYShowBadHTML("Bad HTML: TEXTAREA not used properly *****\n");
	}
	if (TRACE) {
	    HTChunkTerminate(&me->textarea);
	    CTRACE((tfp, "HTML_free: ***** leftover textarea data: %s\n",
		    me->textarea.data));
	}
	HTChunkClear(&me->textarea);
    }

    if (me->target) {
	(*me->targetClass._free) (me->target);
    }
    if (me->sp && me->sp->style && GetHTStyleName(me->sp->style)) {
	if (me->sp->style->id == ST_DivCenter ||
	    me->sp->style->id == ST_HeadingCenter ||
	    me->sp->style->id == ST_Heading1) {
	    me->sp->style->alignment = HT_CENTER;
	} else if (me->sp->style->id == ST_DivRight ||
		   me->sp->style->id == ST_HeadingRight) {
	    me->sp->style->alignment = HT_RIGHT;
	} else {
	    me->sp->style->alignment = HT_LEFT;
	}
	styles[HTML_PRE]->alignment = HT_LEFT;
    }
    FREE(me->base_href);
    FREE(me->map_address);
    FREE(me->LastOptionValue);
    clear_objectdata(me);
    FREE(me);
}
