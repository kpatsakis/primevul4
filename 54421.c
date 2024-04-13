static void HTMLSRC_apply_markup(HTStructured * context, HTlexeme lexeme, int start,
				 int tag_charset)
{
    HT_tagspec *ts = *((start ? lexeme_start : lexeme_end) + lexeme);

    while (ts) {
#ifdef USE_COLOR_STYLE
	if (ts->start) {
	    current_tag_style = ts->style;
	    force_current_tag_style = TRUE;
	    forced_classname = ts->class_name;
	    force_classname = TRUE;
	}
#endif
	CTRACE((tfp, ts->start ? "SRCSTART %d\n" : "SRCSTOP %d\n", (int) lexeme));
	if (ts->start)
	    HTML_start_element(context,
			       (int) ts->element,
			       ts->present,
			       (STRING2PTR) ts->value,
			       tag_charset,
			       NULL);
	else
	    HTML_end_element(context,
			     (int) ts->element,
			     NULL);
	ts = ts->next;
    }
}
