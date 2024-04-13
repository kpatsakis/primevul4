do_search(
    oparg_T	    *oap,	// can be NULL
    int		    dirc,	// '/' or '?'
    int		    search_delim, // the delimiter for the search, e.g. '%' in
				  // s%regex%replacement%
    char_u	    *pat,
    long	    count,
    int		    options,
    searchit_arg_T  *sia)	// optional arguments or NULL
{
    pos_T	    pos;	// position of the last match
    char_u	    *searchstr;
    soffset_T	    old_off;
    int		    retval;	// Return value
    char_u	    *p;
    long	    c;
    char_u	    *dircp;
    char_u	    *strcopy = NULL;
    char_u	    *ps;
    char_u	    *msgbuf = NULL;
    size_t	    len;
    int		    has_offset = FALSE;

    /*
     * A line offset is not remembered, this is vi compatible.
     */
    if (spats[0].off.line && vim_strchr(p_cpo, CPO_LINEOFF) != NULL)
    {
	spats[0].off.line = FALSE;
	spats[0].off.off = 0;
    }

    /*
     * Save the values for when (options & SEARCH_KEEP) is used.
     * (there is no "if ()" around this because gcc wants them initialized)
     */
    old_off = spats[0].off;

    pos = curwin->w_cursor;	// start searching at the cursor position

    /*
     * Find out the direction of the search.
     */
    if (dirc == 0)
	dirc = spats[0].off.dir;
    else
    {
	spats[0].off.dir = dirc;
#if defined(FEAT_EVAL)
	set_vv_searchforward();
#endif
    }
    if (options & SEARCH_REV)
    {
#ifdef MSWIN
	// There is a bug in the Visual C++ 2.2 compiler which means that
	// dirc always ends up being '/'
	dirc = (dirc == '/')  ?  '?'  :  '/';
#else
	if (dirc == '/')
	    dirc = '?';
	else
	    dirc = '/';
#endif
    }

#ifdef FEAT_FOLDING
    // If the cursor is in a closed fold, don't find another match in the same
    // fold.
    if (dirc == '/')
    {
	if (hasFolding(pos.lnum, NULL, &pos.lnum))
	    pos.col = MAXCOL - 2;	// avoid overflow when adding 1
    }
    else
    {
	if (hasFolding(pos.lnum, &pos.lnum, NULL))
	    pos.col = 0;
    }
#endif

#ifdef FEAT_SEARCH_EXTRA
    /*
     * Turn 'hlsearch' highlighting back on.
     */
    if (no_hlsearch && !(options & SEARCH_KEEP))
    {
	redraw_all_later(SOME_VALID);
	set_no_hlsearch(FALSE);
    }
#endif

    /*
     * Repeat the search when pattern followed by ';', e.g. "/foo/;?bar".
     */
    for (;;)
    {
	int		show_top_bot_msg = FALSE;

	searchstr = pat;
	dircp = NULL;
					    // use previous pattern
	if (pat == NULL || *pat == NUL || *pat == search_delim)
	{
	    if (spats[RE_SEARCH].pat == NULL)	    // no previous pattern
	    {
		searchstr = spats[RE_SUBST].pat;
		if (searchstr == NULL)
		{
		    emsg(_(e_no_previous_regular_expression));
		    retval = 0;
		    goto end_do_search;
		}
	    }
	    else
	    {
		// make search_regcomp() use spats[RE_SEARCH].pat
		searchstr = (char_u *)"";
	    }
	}

	if (pat != NULL && *pat != NUL)	// look for (new) offset
	{
	    /*
	     * Find end of regular expression.
	     * If there is a matching '/' or '?', toss it.
	     */
	    ps = strcopy;
	    p = skip_regexp_ex(pat, search_delim, magic_isset(),
							&strcopy, NULL, NULL);
	    if (strcopy != ps)
	    {
		// made a copy of "pat" to change "\?" to "?"
		searchcmdlen += (int)(STRLEN(pat) - STRLEN(strcopy));
		pat = strcopy;
		searchstr = strcopy;
	    }
	    if (*p == search_delim)
	    {
		dircp = p;	// remember where we put the NUL
		*p++ = NUL;
	    }
	    spats[0].off.line = FALSE;
	    spats[0].off.end = FALSE;
	    spats[0].off.off = 0;
	    /*
	     * Check for a line offset or a character offset.
	     * For get_address (echo off) we don't check for a character
	     * offset, because it is meaningless and the 's' could be a
	     * substitute command.
	     */
	    if (*p == '+' || *p == '-' || VIM_ISDIGIT(*p))
		spats[0].off.line = TRUE;
	    else if ((options & SEARCH_OPT)
				      && (*p == 'e' || *p == 's' || *p == 'b'))
	    {
		if (*p == 'e')		// end
		    spats[0].off.end = SEARCH_END;
		++p;
	    }
	    if (VIM_ISDIGIT(*p) || *p == '+' || *p == '-')  // got an offset
	    {
					    // 'nr' or '+nr' or '-nr'
		if (VIM_ISDIGIT(*p) || VIM_ISDIGIT(*(p + 1)))
		    spats[0].off.off = atol((char *)p);
		else if (*p == '-')	    // single '-'
		    spats[0].off.off = -1;
		else			    // single '+'
		    spats[0].off.off = 1;
		++p;
		while (VIM_ISDIGIT(*p))	    // skip number
		    ++p;
	    }

	    // compute length of search command for get_address()
	    searchcmdlen += (int)(p - pat);

	    pat = p;			    // put pat after search command
	}

	if ((options & SEARCH_ECHO) && messaging()
		&& !msg_silent
		&& (!cmd_silent || !shortmess(SHM_SEARCHCOUNT)))
	{
	    char_u	*trunc;
	    char_u	off_buf[40];
	    size_t	off_len = 0;

	    // Compute msg_row early.
	    msg_start();

	    // Get the offset, so we know how long it is.
	    if (!cmd_silent &&
		    (spats[0].off.line || spats[0].off.end || spats[0].off.off))
	    {
		p = off_buf;
		*p++ = dirc;
		if (spats[0].off.end)
		    *p++ = 'e';
		else if (!spats[0].off.line)
		    *p++ = 's';
		if (spats[0].off.off > 0 || spats[0].off.line)
		    *p++ = '+';
		*p = NUL;
		if (spats[0].off.off != 0 || spats[0].off.line)
		    sprintf((char *)p, "%ld", spats[0].off.off);
		off_len = STRLEN(off_buf);
	    }

	    if (*searchstr == NUL)
		p = spats[0].pat;
	    else
		p = searchstr;

	    if (!shortmess(SHM_SEARCHCOUNT) || cmd_silent)
	    {
		// Reserve enough space for the search pattern + offset +
		// search stat.  Use all the space available, so that the
		// search state is right aligned.  If there is not enough space
		// msg_strtrunc() will shorten in the middle.
		if (msg_scrolled != 0 && !cmd_silent)
		    // Use all the columns.
		    len = (int)(Rows - msg_row) * Columns - 1;
		else
		    // Use up to 'showcmd' column.
		    len = (int)(Rows - msg_row - 1) * Columns + sc_col - 1;
		if (len < STRLEN(p) + off_len + SEARCH_STAT_BUF_LEN + 3)
		    len = STRLEN(p) + off_len + SEARCH_STAT_BUF_LEN + 3;
	    }
	    else
		// Reserve enough space for the search pattern + offset.
		len = STRLEN(p) + off_len + 3;

	    vim_free(msgbuf);
	    msgbuf = alloc(len);
	    if (msgbuf != NULL)
	    {
		vim_memset(msgbuf, ' ', len);
		msgbuf[len - 1] = NUL;
		// do not fill the msgbuf buffer, if cmd_silent is set, leave it
		// empty for the search_stat feature.
		if (!cmd_silent)
		{
		    msgbuf[0] = dirc;

		    if (enc_utf8 && utf_iscomposing(utf_ptr2char(p)))
		    {
			// Use a space to draw the composing char on.
			msgbuf[1] = ' ';
			mch_memmove(msgbuf + 2, p, STRLEN(p));
		    }
		    else
			mch_memmove(msgbuf + 1, p, STRLEN(p));
		    if (off_len > 0)
			mch_memmove(msgbuf + STRLEN(p) + 1, off_buf, off_len);

		    trunc = msg_strtrunc(msgbuf, TRUE);
		    if (trunc != NULL)
		    {
			vim_free(msgbuf);
			msgbuf = trunc;
		    }

#ifdef FEAT_RIGHTLEFT
		    // The search pattern could be shown on the right in
		    // rightleft mode, but the 'ruler' and 'showcmd' area use
		    // it too, thus it would be blanked out again very soon.
		    // Show it on the left, but do reverse the text.
		    if (curwin->w_p_rl && *curwin->w_p_rlc == 's')
		    {
			char_u *r;
			size_t pat_len;

			r = reverse_text(msgbuf);
			if (r != NULL)
			{
			    vim_free(msgbuf);
			    msgbuf = r;
			    // move reversed text to beginning of buffer
			    while (*r != NUL && *r == ' ')
				r++;
			    pat_len = msgbuf + STRLEN(msgbuf) - r;
			    mch_memmove(msgbuf, r, pat_len);
			    // overwrite old text
			    if ((size_t)(r - msgbuf) >= pat_len)
				vim_memset(r, ' ', pat_len);
			    else
				vim_memset(msgbuf + pat_len, ' ', r - msgbuf);
			}
		    }
#endif
		    msg_outtrans(msgbuf);
		    msg_clr_eos();
		    msg_check();

		    gotocmdline(FALSE);
		    out_flush();
		    msg_nowait = TRUE;	    // don't wait for this message
		}
	    }
	}

	/*
	 * If there is a character offset, subtract it from the current
	 * position, so we don't get stuck at "?pat?e+2" or "/pat/s-2".
	 * Skip this if pos.col is near MAXCOL (closed fold).
	 * This is not done for a line offset, because then we would not be vi
	 * compatible.
	 */
	if (!spats[0].off.line && spats[0].off.off && pos.col < MAXCOL - 2)
	{
	    if (spats[0].off.off > 0)
	    {
		for (c = spats[0].off.off; c; --c)
		    if (decl(&pos) == -1)
			break;
		if (c)			// at start of buffer
		{
		    pos.lnum = 0;	// allow lnum == 0 here
		    pos.col = MAXCOL;
		}
	    }
	    else
	    {
		for (c = spats[0].off.off; c; ++c)
		    if (incl(&pos) == -1)
			break;
		if (c)			// at end of buffer
		{
		    pos.lnum = curbuf->b_ml.ml_line_count + 1;
		    pos.col = 0;
		}
	    }
	}

	/*
	 * The actual search.
	 */
	c = searchit(curwin, curbuf, &pos, NULL,
					      dirc == '/' ? FORWARD : BACKWARD,
		searchstr, count, spats[0].off.end + (options &
		       (SEARCH_KEEP + SEARCH_PEEK + SEARCH_HIS
			+ SEARCH_MSG + SEARCH_START
			+ ((pat != NULL && *pat == ';') ? 0 : SEARCH_NOOF))),
		RE_LAST, sia);

	if (dircp != NULL)
	    *dircp = search_delim; // restore second '/' or '?' for normal_cmd()

	if (!shortmess(SHM_SEARCH)
		&& ((dirc == '/' && LT_POS(pos, curwin->w_cursor))
			    || (dirc == '?' && LT_POS(curwin->w_cursor, pos))))
	    show_top_bot_msg = TRUE;

	if (c == FAIL)
	{
	    retval = 0;
	    goto end_do_search;
	}
	if (spats[0].off.end && oap != NULL)
	    oap->inclusive = TRUE;  // 'e' includes last character

	retval = 1;		    // pattern found

	/*
	 * Add character and/or line offset
	 */
	if (!(options & SEARCH_NOOF) || (pat != NULL && *pat == ';'))
	{
	    pos_T org_pos = pos;

	    if (spats[0].off.line)	// Add the offset to the line number.
	    {
		c = pos.lnum + spats[0].off.off;
		if (c < 1)
		    pos.lnum = 1;
		else if (c > curbuf->b_ml.ml_line_count)
		    pos.lnum = curbuf->b_ml.ml_line_count;
		else
		    pos.lnum = c;
		pos.col = 0;

		retval = 2;	    // pattern found, line offset added
	    }
	    else if (pos.col < MAXCOL - 2)	// just in case
	    {
		// to the right, check for end of file
		c = spats[0].off.off;
		if (c > 0)
		{
		    while (c-- > 0)
			if (incl(&pos) == -1)
			    break;
		}
		// to the left, check for start of file
		else
		{
		    while (c++ < 0)
			if (decl(&pos) == -1)
			    break;
		}
	    }
	    if (!EQUAL_POS(pos, org_pos))
		has_offset = TRUE;
	}

	// Show [1/15] if 'S' is not in 'shortmess'.
	if ((options & SEARCH_ECHO)
		&& messaging()
		&& !msg_silent
		&& c != FAIL
		&& !shortmess(SHM_SEARCHCOUNT)
		&& msgbuf != NULL)
	     cmdline_search_stat(dirc, &pos, &curwin->w_cursor,
				show_top_bot_msg, msgbuf,
				(count != 1 || has_offset
#ifdef FEAT_FOLDING
				 || (!(fdo_flags & FDO_SEARCH)
				     && hasFolding(curwin->w_cursor.lnum,
								   NULL, NULL))
#endif
				),
				SEARCH_STAT_DEF_MAX_COUNT,
				SEARCH_STAT_DEF_TIMEOUT);

	/*
	 * The search command can be followed by a ';' to do another search.
	 * For example: "/pat/;/foo/+3;?bar"
	 * This is like doing another search command, except:
	 * - The remembered direction '/' or '?' is from the first search.
	 * - When an error happens the cursor isn't moved at all.
	 * Don't do this when called by get_address() (it handles ';' itself).
	 */
	if (!(options & SEARCH_OPT) || pat == NULL || *pat != ';')
	    break;

	dirc = *++pat;
	search_delim = dirc;
	if (dirc != '?' && dirc != '/')
	{
	    retval = 0;
	    emsg(_(e_expected_question_or_slash_after_semicolon));
	    goto end_do_search;
	}
	++pat;
    }

    if (options & SEARCH_MARK)
	setpcmark();
    curwin->w_cursor = pos;
    curwin->w_set_curswant = TRUE;

end_do_search:
    if ((options & SEARCH_KEEP) || (cmdmod.cmod_flags & CMOD_KEEPPATTERNS))
	spats[0].off = old_off;
    vim_free(strcopy);
    vim_free(msgbuf);

    return retval;
}