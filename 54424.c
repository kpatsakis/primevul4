static void HTML_abort(HTStructured * me, HTError e)
{
    char *include = NULL;

    if (me->text) {
	/*
	 * If we have emphasis on, or open A, FONT, or FORM containers, turn it
	 * off or close them now.  - FM
	 */
	if (me->inUnderline) {
	    HText_appendCharacter(me->text, LY_UNDERLINE_END_CHAR);
	    me->inUnderline = FALSE;
	    me->Underline_Level = 0;
	}
	if (me->inA) {
	    HTML_end_element(me, HTML_A, &include);
	    me->inA = FALSE;
	}
	if (me->inFONT) {
	    HTML_end_element(me, HTML_FONT, &include);
	    me->inFONT = FALSE;
	}
	if (me->inFORM) {
	    HTML_end_element(me, HTML_FORM, &include);
	    me->inFORM = FALSE;
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
	if (TRACE) {
	    CTRACE((tfp,
		    "HTML_abort: SELECT or OPTION not ended properly *****\n"));
	    HTChunkTerminate(&me->option);
	    CTRACE((tfp, "HTML_abort: ***** leftover option data: %s\n",
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
	if (TRACE) {
	    CTRACE((tfp, "HTML_abort: TEXTAREA not used properly *****\n"));
	    HTChunkTerminate(&me->textarea);
	    CTRACE((tfp, "HTML_abort: ***** leftover textarea data: %s\n",
		    me->textarea.data));
	}
	HTChunkClear(&me->textarea);
    }

    if (me->target) {
	(*me->targetClass._abort) (me->target, e);
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
    FREE(me->textarea_name);
    FREE(me->textarea_accept_cs);
    FREE(me->textarea_id);
    FREE(me->LastOptionValue);
    FREE(me->xinclude);
    clear_objectdata(me);
    FREE(me);
}
