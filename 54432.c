BOOL LYBadHTML(HTStructured * me)
{
    BOOL code = FALSE;

    switch ((enumBadHtml) cfg_bad_html) {
    case BAD_HTML_IGNORE:
	break;
    case BAD_HTML_TRACE:
	code = TRUE;
	break;
    case BAD_HTML_MESSAGE:
	code = TRUE;
	break;
    case BAD_HTML_WARN:
	/*
	 * If we're already tracing, do not add a warning.
	 */
	if (!TRACE && !me->inBadHTML) {
	    HTUserMsg(BAD_HTML_USE_TRACE);
	    me->inBadHTML = TRUE;
	}
	code = TRACE;
	break;
    }
    return code;
}
