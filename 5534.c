findmatchlimit(
    oparg_T	*oap,
    int		initc,
    int		flags,
    int		maxtravel)
{
    static pos_T pos;			// current search position
    int		findc = 0;		// matching brace
    int		c;
    int		count = 0;		// cumulative number of braces
    int		backwards = FALSE;	// init for gcc
    int		raw_string = FALSE;	// search for raw string
    int		inquote = FALSE;	// TRUE when inside quotes
    char_u	*linep;			// pointer to current line
    char_u	*ptr;
    int		do_quotes;		// check for quotes in current line
    int		at_start;		// do_quotes value at start position
    int		hash_dir = 0;		// Direction searched for # things
    int		comment_dir = 0;	// Direction searched for comments
    pos_T	match_pos;		// Where last slash-star was found
    int		start_in_quotes;	// start position is in quotes
    int		traveled = 0;		// how far we've searched so far
    int		ignore_cend = FALSE;    // ignore comment end
    int		cpo_match;		// vi compatible matching
    int		cpo_bsl;		// don't recognize backslashes
    int		match_escaped = 0;	// search for escaped match
    int		dir;			// Direction to search
    int		comment_col = MAXCOL;   // start of / / comment
    int		lispcomm = FALSE;	// inside of Lisp-style comment
    int		lisp = curbuf->b_p_lisp; // engage Lisp-specific hacks ;)

    pos = curwin->w_cursor;
    pos.coladd = 0;
    linep = ml_get(pos.lnum);

    cpo_match = (vim_strchr(p_cpo, CPO_MATCH) != NULL);
    cpo_bsl = (vim_strchr(p_cpo, CPO_MATCHBSL) != NULL);

    // Direction to search when initc is '/', '*' or '#'
    if (flags & FM_BACKWARD)
	dir = BACKWARD;
    else if (flags & FM_FORWARD)
	dir = FORWARD;
    else
	dir = 0;

    /*
     * if initc given, look in the table for the matching character
     * '/' and '*' are special cases: look for start or end of comment.
     * When '/' is used, we ignore running backwards into an star-slash, for
     * "[*" command, we just want to find any comment.
     */
    if (initc == '/' || initc == '*' || initc == 'R')
    {
	comment_dir = dir;
	if (initc == '/')
	    ignore_cend = TRUE;
	backwards = (dir == FORWARD) ? FALSE : TRUE;
	raw_string = (initc == 'R');
	initc = NUL;
    }
    else if (initc != '#' && initc != NUL)
    {
	find_mps_values(&initc, &findc, &backwards, TRUE);
	if (dir)
	    backwards = (dir == FORWARD) ? FALSE : TRUE;
	if (findc == NUL)
	    return NULL;
    }
    else
    {
	/*
	 * Either initc is '#', or no initc was given and we need to look
	 * under the cursor.
	 */
	if (initc == '#')
	{
	    hash_dir = dir;
	}
	else
	{
	    /*
	     * initc was not given, must look for something to match under
	     * or near the cursor.
	     * Only check for special things when 'cpo' doesn't have '%'.
	     */
	    if (!cpo_match)
	    {
		// Are we before or at #if, #else etc.?
		ptr = skipwhite(linep);
		if (*ptr == '#' && pos.col <= (colnr_T)(ptr - linep))
		{
		    ptr = skipwhite(ptr + 1);
		    if (   STRNCMP(ptr, "if", 2) == 0
			|| STRNCMP(ptr, "endif", 5) == 0
			|| STRNCMP(ptr, "el", 2) == 0)
			hash_dir = 1;
		}

		// Are we on a comment?
		else if (linep[pos.col] == '/')
		{
		    if (linep[pos.col + 1] == '*')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
			pos.col++;
		    }
		    else if (pos.col > 0 && linep[pos.col - 1] == '*')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
			pos.col--;
		    }
		}
		else if (linep[pos.col] == '*')
		{
		    if (linep[pos.col + 1] == '/')
		    {
			comment_dir = BACKWARD;
			backwards = TRUE;
		    }
		    else if (pos.col > 0 && linep[pos.col - 1] == '/')
		    {
			comment_dir = FORWARD;
			backwards = FALSE;
		    }
		}
	    }

	    /*
	     * If we are not on a comment or the # at the start of a line, then
	     * look for brace anywhere on this line after the cursor.
	     */
	    if (!hash_dir && !comment_dir)
	    {
		/*
		 * Find the brace under or after the cursor.
		 * If beyond the end of the line, use the last character in
		 * the line.
		 */
		if (linep[pos.col] == NUL && pos.col)
		    --pos.col;
		for (;;)
		{
		    initc = PTR2CHAR(linep + pos.col);
		    if (initc == NUL)
			break;

		    find_mps_values(&initc, &findc, &backwards, FALSE);
		    if (findc)
			break;
		    pos.col += mb_ptr2len(linep + pos.col);
		}
		if (!findc)
		{
		    // no brace in the line, maybe use "  #if" then
		    if (!cpo_match && *skipwhite(linep) == '#')
			hash_dir = 1;
		    else
			return NULL;
		}
		else if (!cpo_bsl)
		{
		    int col, bslcnt = 0;

		    // Set "match_escaped" if there are an odd number of
		    // backslashes.
		    for (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		    match_escaped = (bslcnt & 1);
		}
	    }
	}
	if (hash_dir)
	{
	    /*
	     * Look for matching #if, #else, #elif, or #endif
	     */
	    if (oap != NULL)
		oap->motion_type = MLINE;   // Linewise for this case only
	    if (initc != '#')
	    {
		ptr = skipwhite(skipwhite(linep) + 1);
		if (STRNCMP(ptr, "if", 2) == 0 || STRNCMP(ptr, "el", 2) == 0)
		    hash_dir = 1;
		else if (STRNCMP(ptr, "endif", 5) == 0)
		    hash_dir = -1;
		else
		    return NULL;
	    }
	    pos.col = 0;
	    while (!got_int)
	    {
		if (hash_dir > 0)
		{
		    if (pos.lnum == curbuf->b_ml.ml_line_count)
			break;
		}
		else if (pos.lnum == 1)
		    break;
		pos.lnum += hash_dir;
		linep = ml_get(pos.lnum);
		line_breakcheck();	// check for CTRL-C typed
		ptr = skipwhite(linep);
		if (*ptr != '#')
		    continue;
		pos.col = (colnr_T) (ptr - linep);
		ptr = skipwhite(ptr + 1);
		if (hash_dir > 0)
		{
		    if (STRNCMP(ptr, "if", 2) == 0)
			count++;
		    else if (STRNCMP(ptr, "el", 2) == 0)
		    {
			if (count == 0)
			    return &pos;
		    }
		    else if (STRNCMP(ptr, "endif", 5) == 0)
		    {
			if (count == 0)
			    return &pos;
			count--;
		    }
		}
		else
		{
		    if (STRNCMP(ptr, "if", 2) == 0)
		    {
			if (count == 0)
			    return &pos;
			count--;
		    }
		    else if (initc == '#' && STRNCMP(ptr, "el", 2) == 0)
		    {
			if (count == 0)
			    return &pos;
		    }
		    else if (STRNCMP(ptr, "endif", 5) == 0)
			count++;
		}
	    }
	    return NULL;
	}
    }

#ifdef FEAT_RIGHTLEFT
    // This is just guessing: when 'rightleft' is set, search for a matching
    // paren/brace in the other direction.
    if (curwin->w_p_rl && vim_strchr((char_u *)"()[]{}<>", initc) != NULL)
	backwards = !backwards;
#endif

    do_quotes = -1;
    start_in_quotes = MAYBE;
    CLEAR_POS(&match_pos);

    // backward search: Check if this line contains a single-line comment
    if ((backwards && comment_dir) || lisp)
	comment_col = check_linecomment(linep);
    if (lisp && comment_col != MAXCOL && pos.col > (colnr_T)comment_col)
	lispcomm = TRUE;    // find match inside this comment

    while (!got_int)
    {
	/*
	 * Go to the next position, forward or backward. We could use
	 * inc() and dec() here, but that is much slower
	 */
	if (backwards)
	{
	    // char to match is inside of comment, don't search outside
	    if (lispcomm && pos.col < (colnr_T)comment_col)
		break;
	    if (pos.col == 0)		// at start of line, go to prev. one
	    {
		if (pos.lnum == 1)	// start of file
		    break;
		--pos.lnum;

		if (maxtravel > 0 && ++traveled > maxtravel)
		    break;

		linep = ml_get(pos.lnum);
		pos.col = (colnr_T)STRLEN(linep); // pos.col on trailing NUL
		do_quotes = -1;
		line_breakcheck();

		// Check if this line contains a single-line comment
		if (comment_dir || lisp)
		    comment_col = check_linecomment(linep);
		// skip comment
		if (lisp && comment_col != MAXCOL)
		    pos.col = comment_col;
	    }
	    else
	    {
		--pos.col;
		if (has_mbyte)
		    pos.col -= (*mb_head_off)(linep, linep + pos.col);
	    }
	}
	else				// forward search
	{
	    if (linep[pos.col] == NUL
		    // at end of line, go to next one
		    // For lisp don't search for match in comment
		    || (lisp && comment_col != MAXCOL
					   && pos.col == (colnr_T)comment_col))
	    {
		if (pos.lnum == curbuf->b_ml.ml_line_count  // end of file
			// line is exhausted and comment with it,
			// don't search for match in code
			 || lispcomm)
		    break;
		++pos.lnum;

		if (maxtravel && traveled++ > maxtravel)
		    break;

		linep = ml_get(pos.lnum);
		pos.col = 0;
		do_quotes = -1;
		line_breakcheck();
		if (lisp)   // find comment pos in new line
		    comment_col = check_linecomment(linep);
	    }
	    else
	    {
		if (has_mbyte)
		    pos.col += (*mb_ptr2len)(linep + pos.col);
		else
		    ++pos.col;
	    }
	}

	/*
	 * If FM_BLOCKSTOP given, stop at a '{' or '}' in column 0.
	 */
	if (pos.col == 0 && (flags & FM_BLOCKSTOP)
				       && (linep[0] == '{' || linep[0] == '}'))
	{
	    if (linep[0] == findc && count == 0)	// match!
		return &pos;
	    break;					// out of scope
	}

	if (comment_dir)
	{
	    // Note: comments do not nest, and we ignore quotes in them
	    // TODO: ignore comment brackets inside strings
	    if (comment_dir == FORWARD)
	    {
		if (linep[pos.col] == '*' && linep[pos.col + 1] == '/')
		{
		    pos.col++;
		    return &pos;
		}
	    }
	    else    // Searching backwards
	    {
		/*
		 * A comment may contain / * or / /, it may also start or end
		 * with / * /.	Ignore a / * after / / and after *.
		 */
		if (pos.col == 0)
		    continue;
		else if (raw_string)
		{
		    if (linep[pos.col - 1] == 'R'
			&& linep[pos.col] == '"'
			&& vim_strchr(linep + pos.col + 1, '(') != NULL)
		    {
			// Possible start of raw string. Now that we have the
			// delimiter we can check if it ends before where we
			// started searching, or before the previously found
			// raw string start.
			if (!find_rawstring_end(linep, &pos,
				  count > 0 ? &match_pos : &curwin->w_cursor))
			{
			    count++;
			    match_pos = pos;
			    match_pos.col--;
			}
			linep = ml_get(pos.lnum); // may have been released
		    }
		}
		else if (  linep[pos.col - 1] == '/'
			&& linep[pos.col] == '*'
			&& (pos.col == 1 || linep[pos.col - 2] != '*')
			&& (int)pos.col < comment_col)
		{
		    count++;
		    match_pos = pos;
		    match_pos.col--;
		}
		else if (linep[pos.col - 1] == '*' && linep[pos.col] == '/')
		{
		    if (count > 0)
			pos = match_pos;
		    else if (pos.col > 1 && linep[pos.col - 2] == '/'
					       && (int)pos.col <= comment_col)
			pos.col -= 2;
		    else if (ignore_cend)
			continue;
		    else
			return NULL;
		    return &pos;
		}
	    }
	    continue;
	}

	/*
	 * If smart matching ('cpoptions' does not contain '%'), braces inside
	 * of quotes are ignored, but only if there is an even number of
	 * quotes in the line.
	 */
	if (cpo_match)
	    do_quotes = 0;
	else if (do_quotes == -1)
	{
	    /*
	     * Count the number of quotes in the line, skipping \" and '"'.
	     * Watch out for "\\".
	     */
	    at_start = do_quotes;
	    for (ptr = linep; *ptr; ++ptr)
	    {
		if (ptr == linep + pos.col + backwards)
		    at_start = (do_quotes & 1);
		if (*ptr == '"'
			&& (ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
		    ++do_quotes;
		if (*ptr == '\\' && ptr[1] != NUL)
		    ++ptr;
	    }
	    do_quotes &= 1;	    // result is 1 with even number of quotes

	    /*
	     * If we find an uneven count, check current line and previous
	     * one for a '\' at the end.
	     */
	    if (!do_quotes)
	    {
		inquote = FALSE;
		if (ptr[-1] == '\\')
		{
		    do_quotes = 1;
		    if (start_in_quotes == MAYBE)
		    {
			// Do we need to use at_start here?
			inquote = TRUE;
			start_in_quotes = TRUE;
		    }
		    else if (backwards)
			inquote = TRUE;
		}
		if (pos.lnum > 1)
		{
		    ptr = ml_get(pos.lnum - 1);
		    if (*ptr && *(ptr + STRLEN(ptr) - 1) == '\\')
		    {
			do_quotes = 1;
			if (start_in_quotes == MAYBE)
			{
			    inquote = at_start;
			    if (inquote)
				start_in_quotes = TRUE;
			}
			else if (!backwards)
			    inquote = TRUE;
		    }

		    // ml_get() only keeps one line, need to get linep again
		    linep = ml_get(pos.lnum);
		}
	    }
	}
	if (start_in_quotes == MAYBE)
	    start_in_quotes = FALSE;

	/*
	 * If 'smartmatch' is set:
	 *   Things inside quotes are ignored by setting 'inquote'.  If we
	 *   find a quote without a preceding '\' invert 'inquote'.  At the
	 *   end of a line not ending in '\' we reset 'inquote'.
	 *
	 *   In lines with an uneven number of quotes (without preceding '\')
	 *   we do not know which part to ignore. Therefore we only set
	 *   inquote if the number of quotes in a line is even, unless this
	 *   line or the previous one ends in a '\'.  Complicated, isn't it?
	 */
	c = PTR2CHAR(linep + pos.col);
	switch (c)
	{
	case NUL:
	    // at end of line without trailing backslash, reset inquote
	    if (pos.col == 0 || linep[pos.col - 1] != '\\')
	    {
		inquote = FALSE;
		start_in_quotes = FALSE;
	    }
	    break;

	case '"':
	    // a quote that is preceded with an odd number of backslashes is
	    // ignored
	    if (do_quotes)
	    {
		int col;

		for (col = pos.col - 1; col >= 0; --col)
		    if (linep[col] != '\\')
			break;
		if ((((int)pos.col - 1 - col) & 1) == 0)
		{
		    inquote = !inquote;
		    start_in_quotes = FALSE;
		}
	    }
	    break;

	/*
	 * If smart matching ('cpoptions' does not contain '%'):
	 *   Skip things in single quotes: 'x' or '\x'.  Be careful for single
	 *   single quotes, eg jon's.  Things like '\233' or '\x3f' are not
	 *   skipped, there is never a brace in them.
	 *   Ignore this when finding matches for `'.
	 */
	case '\'':
	    if (!cpo_match && initc != '\'' && findc != '\'')
	    {
		if (backwards)
		{
		    if (pos.col > 1)
		    {
			if (linep[pos.col - 2] == '\'')
			{
			    pos.col -= 2;
			    break;
			}
			else if (linep[pos.col - 2] == '\\'
				  && pos.col > 2 && linep[pos.col - 3] == '\'')
			{
			    pos.col -= 3;
			    break;
			}
		    }
		}
		else if (linep[pos.col + 1])	// forward search
		{
		    if (linep[pos.col + 1] == '\\'
			   && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
		    {
			pos.col += 3;
			break;
		    }
		    else if (linep[pos.col + 2] == '\'')
		    {
			pos.col += 2;
			break;
		    }
		}
	    }
	    // FALLTHROUGH

	default:
	    /*
	     * For Lisp skip over backslashed (), {} and [].
	     * (actually, we skip #\( et al)
	     */
	    if (curbuf->b_p_lisp
		    && vim_strchr((char_u *)"(){}[]", c) != NULL
		    && pos.col > 1
		    && check_prevcol(linep, pos.col, '\\', NULL)
		    && check_prevcol(linep, pos.col - 1, '#', NULL))
		break;

	    // Check for match outside of quotes, and inside of
	    // quotes when the start is also inside of quotes.
	    if ((!inquote || start_in_quotes == TRUE)
		    && (c == initc || c == findc))
	    {
		int	col, bslcnt = 0;

		if (!cpo_bsl)
		{
		    for (col = pos.col; check_prevcol(linep, col, '\\', &col);)
			bslcnt++;
		}
		// Only accept a match when 'M' is in 'cpo' or when escaping
		// is what we expect.
		if (cpo_bsl || (bslcnt & 1) == match_escaped)
		{
		    if (c == initc)
			count++;
		    else
		    {
			if (count == 0)
			    return &pos;
			count--;
		    }
		}
	    }
	}
    }

    if (comment_dir == BACKWARD && count > 0)
    {
	pos = match_pos;
	return &pos;
    }
    return (pos_T *)NULL;	// never found it
}