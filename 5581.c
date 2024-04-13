searchit(
    win_T	*win,		// window to search in; can be NULL for a
				// buffer without a window!
    buf_T	*buf,
    pos_T	*pos,
    pos_T	*end_pos,	// set to end of the match, unless NULL
    int		dir,
    char_u	*pat,
    long	count,
    int		options,
    int		pat_use,	// which pattern to use when "pat" is empty
    searchit_arg_T *extra_arg)	// optional extra arguments, can be NULL
{
    int		found;
    linenr_T	lnum;		// no init to shut up Apollo cc
    colnr_T	col;
    regmmatch_T	regmatch;
    char_u	*ptr;
    colnr_T	matchcol;
    lpos_T	endpos;
    lpos_T	matchpos;
    int		loop;
    pos_T	start_pos;
    int		at_first_line;
    int		extra_col;
    int		start_char_len;
    int		match_ok;
    long	nmatched;
    int		submatch = 0;
    int		first_match = TRUE;
    int		called_emsg_before = called_emsg;
#ifdef FEAT_SEARCH_EXTRA
    int		break_loop = FALSE;
#endif
    linenr_T	stop_lnum = 0;	// stop after this line number when != 0
#ifdef FEAT_RELTIME
    proftime_T	*tm = NULL;	// timeout limit or NULL
    int		*timed_out = NULL;  // set when timed out or NULL
#endif

    if (extra_arg != NULL)
    {
	stop_lnum = extra_arg->sa_stop_lnum;
#ifdef FEAT_RELTIME
	tm = extra_arg->sa_tm;
	timed_out = &extra_arg->sa_timed_out;
#endif
    }

    if (search_regcomp(pat, RE_SEARCH, pat_use,
		   (options & (SEARCH_HIS + SEARCH_KEEP)), &regmatch) == FAIL)
    {
	if ((options & SEARCH_MSG) && !rc_did_emsg)
	    semsg(_(e_invalid_search_string_str), mr_pattern);
	return FAIL;
    }

    /*
     * find the string
     */
    do	// loop for count
    {
	// When not accepting a match at the start position set "extra_col" to
	// a non-zero value.  Don't do that when starting at MAXCOL, since
	// MAXCOL + 1 is zero.
	if (pos->col == MAXCOL)
	    start_char_len = 0;
	// Watch out for the "col" being MAXCOL - 2, used in a closed fold.
	else if (has_mbyte
		    && pos->lnum >= 1 && pos->lnum <= buf->b_ml.ml_line_count
						    && pos->col < MAXCOL - 2)
	{
	    ptr = ml_get_buf(buf, pos->lnum, FALSE);
	    if ((int)STRLEN(ptr) <= pos->col)
		start_char_len = 1;
	    else
		start_char_len = (*mb_ptr2len)(ptr + pos->col);
	}
	else
	    start_char_len = 1;
	if (dir == FORWARD)
	{
	    if (options & SEARCH_START)
		extra_col = 0;
	    else
		extra_col = start_char_len;
	}
	else
	{
	    if (options & SEARCH_START)
		extra_col = start_char_len;
	    else
		extra_col = 0;
	}

	start_pos = *pos;	// remember start pos for detecting no match
	found = 0;		// default: not found
	at_first_line = TRUE;	// default: start in first line
	if (pos->lnum == 0)	// correct lnum for when starting in line 0
	{
	    pos->lnum = 1;
	    pos->col = 0;
	    at_first_line = FALSE;  // not in first line now
	}

	/*
	 * Start searching in current line, unless searching backwards and
	 * we're in column 0.
	 * If we are searching backwards, in column 0, and not including the
	 * current position, gain some efficiency by skipping back a line.
	 * Otherwise begin the search in the current line.
	 */
	if (dir == BACKWARD && start_pos.col == 0
					     && (options & SEARCH_START) == 0)
	{
	    lnum = pos->lnum - 1;
	    at_first_line = FALSE;
	}
	else
	    lnum = pos->lnum;

	for (loop = 0; loop <= 1; ++loop)   // loop twice if 'wrapscan' set
	{
	    for ( ; lnum > 0 && lnum <= buf->b_ml.ml_line_count;
					   lnum += dir, at_first_line = FALSE)
	    {
		// Stop after checking "stop_lnum", if it's set.
		if (stop_lnum != 0 && (dir == FORWARD
				       ? lnum > stop_lnum : lnum < stop_lnum))
		    break;
#ifdef FEAT_RELTIME
		// Stop after passing the "tm" time limit.
		if (tm != NULL && profile_passed_limit(tm))
		    break;
#endif

		/*
		 * Look for a match somewhere in line "lnum".
		 */
		col = at_first_line && (options & SEARCH_COL) ? pos->col
								 : (colnr_T)0;
		nmatched = vim_regexec_multi(&regmatch, win, buf,
					     lnum, col,
#ifdef FEAT_RELTIME
					     tm, timed_out
#else
					     NULL, NULL
#endif
						      );
		// vim_regexec_multi() may clear "regprog"
		if (regmatch.regprog == NULL)
		    break;
		// Abort searching on an error (e.g., out of stack).
		if (called_emsg > called_emsg_before
#ifdef FEAT_RELTIME
			|| (timed_out != NULL && *timed_out)
#endif
			)
		    break;
		if (nmatched > 0)
		{
		    // match may actually be in another line when using \zs
		    matchpos = regmatch.startpos[0];
		    endpos = regmatch.endpos[0];
#ifdef FEAT_EVAL
		    submatch = first_submatch(&regmatch);
#endif
		    // "lnum" may be past end of buffer for "\n\zs".
		    if (lnum + matchpos.lnum > buf->b_ml.ml_line_count)
			ptr = (char_u *)"";
		    else
			ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);

		    /*
		     * Forward search in the first line: match should be after
		     * the start position. If not, continue at the end of the
		     * match (this is vi compatible) or on the next char.
		     */
		    if (dir == FORWARD && at_first_line)
		    {
			match_ok = TRUE;
			/*
			 * When the match starts in a next line it's certainly
			 * past the start position.
			 * When match lands on a NUL the cursor will be put
			 * one back afterwards, compare with that position,
			 * otherwise "/$" will get stuck on end of line.
			 */
			while (matchpos.lnum == 0
				&& ((options & SEARCH_END) && first_match
				    ?  (nmatched == 1
					&& (int)endpos.col - 1
					     < (int)start_pos.col + extra_col)
				    : ((int)matchpos.col
						  - (ptr[matchpos.col] == NUL)
					    < (int)start_pos.col + extra_col)))
			{
			    /*
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    if (vim_strchr(p_cpo, CPO_SEARCH) != NULL)
			    {
				if (nmatched > 1)
				{
				    // end is in next line, thus no match in
				    // this line
				    match_ok = FALSE;
				    break;
				}
				matchcol = endpos.col;
				// for empty match: advance one char
				if (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    if (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    else
					++matchcol;
				}
			    }
			    else
			    {
				matchcol = matchpos.col;
				if (ptr[matchcol] != NUL)
				{
				    if (has_mbyte)
					matchcol += (*mb_ptr2len)(ptr
								  + matchcol);
				    else
					++matchcol;
				}
			    }
			    if (matchcol == 0 && (options & SEARCH_START))
				break;
			    if (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol,
#ifdef FEAT_RELTIME
					      tm, timed_out
#else
					      NULL, NULL
#endif
					      )) == 0)
			    {
				match_ok = FALSE;
				break;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    if (regmatch.regprog == NULL)
				break;
			    matchpos = regmatch.startpos[0];
			    endpos = regmatch.endpos[0];
# ifdef FEAT_EVAL
			    submatch = first_submatch(&regmatch);
# endif

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}
			if (!match_ok)
			    continue;
		    }
		    if (dir == BACKWARD)
		    {
			/*
			 * Now, if there are multiple matches on this line,
			 * we have to get the last one. Or the last one before
			 * the cursor, if we're on that line.
			 * When putting the new cursor at the end, compare
			 * relative to the end of the match.
			 */
			match_ok = FALSE;
			for (;;)
			{
			    // Remember a position that is before the start
			    // position, we use it if it's the last match in
			    // the line.  Always accept a position after
			    // wrapping around.
			    if (loop
				|| ((options & SEARCH_END)
				    ? (lnum + regmatch.endpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.endpos[0].lnum
							     == start_pos.lnum
					     && (int)regmatch.endpos[0].col - 1
							< (int)start_pos.col
								+ extra_col))
				    : (lnum + regmatch.startpos[0].lnum
							      < start_pos.lnum
					|| (lnum + regmatch.startpos[0].lnum
							     == start_pos.lnum
					     && (int)regmatch.startpos[0].col
						      < (int)start_pos.col
							      + extra_col))))
			    {
				match_ok = TRUE;
				matchpos = regmatch.startpos[0];
				endpos = regmatch.endpos[0];
# ifdef FEAT_EVAL
				submatch = first_submatch(&regmatch);
# endif
			    }
			    else
				break;

			    /*
			     * We found a valid match, now check if there is
			     * another one after it.
			     * If vi-compatible searching, continue at the end
			     * of the match, otherwise continue one position
			     * forward.
			     */
			    if (vim_strchr(p_cpo, CPO_SEARCH) != NULL)
			    {
				if (nmatched > 1)
				    break;
				matchcol = endpos.col;
				// for empty match: advance one char
				if (matchcol == matchpos.col
						      && ptr[matchcol] != NUL)
				{
				    if (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    else
					++matchcol;
				}
			    }
			    else
			    {
				// Stop when the match is in a next line.
				if (matchpos.lnum > 0)
				    break;
				matchcol = matchpos.col;
				if (ptr[matchcol] != NUL)
				{
				    if (has_mbyte)
					matchcol +=
					  (*mb_ptr2len)(ptr + matchcol);
				    else
					++matchcol;
				}
			    }
			    if (ptr[matchcol] == NUL
				    || (nmatched = vim_regexec_multi(&regmatch,
					      win, buf, lnum + matchpos.lnum,
					      matchcol,
#ifdef FEAT_RELTIME
					      tm, timed_out
#else
					      NULL, NULL
#endif
					    )) == 0)
			    {
#ifdef FEAT_RELTIME
				// If the search timed out, we did find a match
				// but it might be the wrong one, so that's not
				// OK.
				if (timed_out != NULL && *timed_out)
				    match_ok = FALSE;
#endif
				break;
			    }
			    // vim_regexec_multi() may clear "regprog"
			    if (regmatch.regprog == NULL)
				break;

			    // Need to get the line pointer again, a
			    // multi-line search may have made it invalid.
			    ptr = ml_get_buf(buf, lnum + matchpos.lnum, FALSE);
			}

			/*
			 * If there is only a match after the cursor, skip
			 * this match.
			 */
			if (!match_ok)
			    continue;
		    }

		    // With the SEARCH_END option move to the last character
		    // of the match.  Don't do it for an empty match, end
		    // should be same as start then.
		    if ((options & SEARCH_END) && !(options & SEARCH_NOOF)
			    && !(matchpos.lnum == endpos.lnum
				&& matchpos.col == endpos.col))
		    {
			// For a match in the first column, set the position
			// on the NUL in the previous line.
			pos->lnum = lnum + endpos.lnum;
			pos->col = endpos.col;
			if (endpos.col == 0)
			{
			    if (pos->lnum > 1)  // just in case
			    {
				--pos->lnum;
				pos->col = (colnr_T)STRLEN(ml_get_buf(buf,
							   pos->lnum, FALSE));
			    }
			}
			else
			{
			    --pos->col;
			    if (has_mbyte
				    && pos->lnum <= buf->b_ml.ml_line_count)
			    {
				ptr = ml_get_buf(buf, pos->lnum, FALSE);
				pos->col -= (*mb_head_off)(ptr, ptr + pos->col);
			    }
			}
			if (end_pos != NULL)
			{
			    end_pos->lnum = lnum + matchpos.lnum;
			    end_pos->col = matchpos.col;
			}
		    }
		    else
		    {
			pos->lnum = lnum + matchpos.lnum;
			pos->col = matchpos.col;
			if (end_pos != NULL)
			{
			    end_pos->lnum = lnum + endpos.lnum;
			    end_pos->col = endpos.col;
			}
		    }
		    pos->coladd = 0;
		    if (end_pos != NULL)
			end_pos->coladd = 0;
		    found = 1;
		    first_match = FALSE;

		    // Set variables used for 'incsearch' highlighting.
		    search_match_lines = endpos.lnum - matchpos.lnum;
		    search_match_endcol = endpos.col;
		    break;
		}
		line_breakcheck();	// stop if ctrl-C typed
		if (got_int)
		    break;

#ifdef FEAT_SEARCH_EXTRA
		// Cancel searching if a character was typed.  Used for
		// 'incsearch'.  Don't check too often, that would slowdown
		// searching too much.
		if ((options & SEARCH_PEEK)
			&& ((lnum - pos->lnum) & 0x3f) == 0
			&& char_avail())
		{
		    break_loop = TRUE;
		    break;
		}
#endif

		if (loop && lnum == start_pos.lnum)
		    break;	    // if second loop, stop where started
	    }
	    at_first_line = FALSE;

	    // vim_regexec_multi() may clear "regprog"
	    if (regmatch.regprog == NULL)
		break;

	    /*
	     * Stop the search if wrapscan isn't set, "stop_lnum" is
	     * specified, after an interrupt, after a match and after looping
	     * twice.
	     */
	    if (!p_ws || stop_lnum != 0 || got_int
					    || called_emsg > called_emsg_before
#ifdef FEAT_RELTIME
				|| (timed_out != NULL && *timed_out)
#endif
#ifdef FEAT_SEARCH_EXTRA
				|| break_loop
#endif
				|| found || loop)
		break;

	    /*
	     * If 'wrapscan' is set we continue at the other end of the file.
	     * If 'shortmess' does not contain 's', we give a message.
	     * This message is also remembered in keep_msg for when the screen
	     * is redrawn. The keep_msg is cleared whenever another message is
	     * written.
	     */
	    if (dir == BACKWARD)    // start second loop at the other end
		lnum = buf->b_ml.ml_line_count;
	    else
		lnum = 1;
	    if (!shortmess(SHM_SEARCH) && (options & SEARCH_MSG))
		give_warning((char_u *)_(dir == BACKWARD
					  ? top_bot_msg : bot_top_msg), TRUE);
	    if (extra_arg != NULL)
		extra_arg->sa_wrapped = TRUE;
	}
	if (got_int || called_emsg > called_emsg_before
#ifdef FEAT_RELTIME
		|| (timed_out != NULL && *timed_out)
#endif
#ifdef FEAT_SEARCH_EXTRA
		|| break_loop
#endif
		)
	    break;
    }
    while (--count > 0 && found);   // stop after count matches or no match

    vim_regfree(regmatch.regprog);

    if (!found)		    // did not find it
    {
	if (got_int)
	    emsg(_(e_interrupted));
	else if ((options & SEARCH_MSG) == SEARCH_MSG)
	{
	    if (p_ws)
		semsg(_(e_pattern_not_found_str), mr_pattern);
	    else if (lnum == 0)
		semsg(_(e_search_hit_top_without_match_for_str), mr_pattern);
	    else
		semsg(_(e_search_hit_bottom_without_match_for_str), mr_pattern);
	}
	return FAIL;
    }

    // A pattern like "\n\zs" may go past the last line.
    if (pos->lnum > buf->b_ml.ml_line_count)
    {
	pos->lnum = buf->b_ml.ml_line_count;
	pos->col = (int)STRLEN(ml_get_buf(buf, pos->lnum, FALSE));
	if (pos->col > 0)
	    --pos->col;
    }

    return submatch + 1;
}