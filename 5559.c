show_pat_in_path(
    char_u  *line,
    int	    type,
    int	    did_show,
    int	    action,
    FILE    *fp,
    linenr_T *lnum,
    long    count)
{
    char_u  *p;

    if (did_show)
	msg_putchar('\n');	// cursor below last one
    else if (!msg_silent)
	gotocmdline(TRUE);	// cursor at status line
    if (got_int)		// 'q' typed at "--more--" message
	return;
    for (;;)
    {
	p = line + STRLEN(line) - 1;
	if (fp != NULL)
	{
	    // We used fgets(), so get rid of newline at end
	    if (p >= line && *p == '\n')
		--p;
	    if (p >= line && *p == '\r')
		--p;
	    *(p + 1) = NUL;
	}
	if (action == ACTION_SHOW_ALL)
	{
	    sprintf((char *)IObuff, "%3ld: ", count);	// show match nr
	    msg_puts((char *)IObuff);
	    sprintf((char *)IObuff, "%4ld", *lnum);	// show line nr
						// Highlight line numbers
	    msg_puts_attr((char *)IObuff, HL_ATTR(HLF_N));
	    msg_puts(" ");
	}
	msg_prt_line(line, FALSE);
	out_flush();			// show one line at a time

	// Definition continues until line that doesn't end with '\'
	if (got_int || type != FIND_DEFINE || p < line || *p != '\\')
	    break;

	if (fp != NULL)
	{
	    if (vim_fgets(line, LSIZE, fp)) // end of file
		break;
	    ++*lnum;
	}
	else
	{
	    if (++*lnum > curbuf->b_ml.ml_line_count)
		break;
	    line = ml_get(*lnum);
	}
	msg_putchar('\n');
    }
}