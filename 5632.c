ex_display(exarg_T *eap)
{
    int		i, n;
    long	j;
    char_u	*p;
    yankreg_T	*yb;
    int		name;
    int		attr;
    char_u	*arg = eap->arg;
    int		clen;
    int		type;

    if (arg != NULL && *arg == NUL)
	arg = NULL;
    attr = HL_ATTR(HLF_8);

    // Highlight title
    msg_puts_title(_("\nType Name Content"));
    for (i = -1; i < NUM_REGISTERS && !got_int; ++i)
    {
	name = get_register_name(i);
	switch (get_reg_type(name, NULL))
	{
	    case MLINE: type = 'l'; break;
	    case MCHAR: type = 'c'; break;
	    default:	type = 'b'; break;
	}
	if (arg != NULL && vim_strchr(arg, name) == NULL
#ifdef ONE_CLIPBOARD
	    // Star register and plus register contain the same thing.
		&& (name != '*' || vim_strchr(arg, '+') == NULL)
#endif
		)
	    continue;	    // did not ask for this register

#ifdef FEAT_CLIPBOARD
	// Adjust register name for "unnamed" in 'clipboard'.
	// When it's a clipboard register, fill it with the current contents
	// of the clipboard.
	adjust_clip_reg(&name);
	(void)may_get_selection(name);
#endif

	if (i == -1)
	{
	    if (y_previous != NULL)
		yb = y_previous;
	    else
		yb = &(y_regs[0]);
	}
	else
	    yb = &(y_regs[i]);

#ifdef FEAT_EVAL
	if (name == MB_TOLOWER(redir_reg)
		|| (redir_reg == '"' && yb == y_previous))
	    continue;	    // do not list register being written to, the
			    // pointer can be freed
#endif

	if (yb->y_array != NULL)
	{
	    int do_show = FALSE;

	    for (j = 0; !do_show && j < yb->y_size; ++j)
		do_show = !message_filtered(yb->y_array[j]);

	    if (do_show || yb->y_size == 0)
	    {
		msg_putchar('\n');
		msg_puts("  ");
		msg_putchar(type);
		msg_puts("  ");
		msg_putchar('"');
		msg_putchar(name);
		msg_puts("   ");

		n = (int)Columns - 11;
		for (j = 0; j < yb->y_size && n > 1; ++j)
		{
		    if (j)
		    {
			msg_puts_attr("^J", attr);
			n -= 2;
		    }
		    for (p = yb->y_array[j]; *p && (n -= ptr2cells(p)) >= 0;
									   ++p)
		    {
			clen = (*mb_ptr2len)(p);
			msg_outtrans_len(p, clen);
			p += clen - 1;
		    }
		}
		if (n > 1 && yb->y_type == MLINE)
		    msg_puts_attr("^J", attr);
		out_flush();		    // show one line at a time
	    }
	    ui_breakcheck();
	}
    }

    // display last inserted text
    if ((p = get_last_insert()) != NULL
		  && (arg == NULL || vim_strchr(arg, '.') != NULL) && !got_int
						      && !message_filtered(p))
    {
	msg_puts("\n  c  \".   ");
	dis_msg(p, TRUE);
    }

    // display last command line
    if (last_cmdline != NULL && (arg == NULL || vim_strchr(arg, ':') != NULL)
			       && !got_int && !message_filtered(last_cmdline))
    {
	msg_puts("\n  c  \":   ");
	dis_msg(last_cmdline, FALSE);
    }

    // display current file name
    if (curbuf->b_fname != NULL
	    && (arg == NULL || vim_strchr(arg, '%') != NULL) && !got_int
					&& !message_filtered(curbuf->b_fname))
    {
	msg_puts("\n  c  \"%   ");
	dis_msg(curbuf->b_fname, FALSE);
    }

    // display alternate file name
    if ((arg == NULL || vim_strchr(arg, '%') != NULL) && !got_int)
    {
	char_u	    *fname;
	linenr_T    dummy;

	if (buflist_name_nr(0, &fname, &dummy) != FAIL
						  && !message_filtered(fname))
	{
	    msg_puts("\n  c  \"#   ");
	    dis_msg(fname, FALSE);
	}
    }

    // display last search pattern
    if (last_search_pat() != NULL
		 && (arg == NULL || vim_strchr(arg, '/') != NULL) && !got_int
				      && !message_filtered(last_search_pat()))
    {
	msg_puts("\n  c  \"/   ");
	dis_msg(last_search_pat(), FALSE);
    }

#ifdef FEAT_EVAL
    // display last used expression
    if (expr_line != NULL && (arg == NULL || vim_strchr(arg, '=') != NULL)
				  && !got_int && !message_filtered(expr_line))
    {
	msg_puts("\n  c  \"=   ");
	dis_msg(expr_line, FALSE);
    }
#endif
}