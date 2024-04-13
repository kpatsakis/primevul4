searchc(cmdarg_T *cap, int t_cmd)
{
    int			c = cap->nchar;	// char to search for
    int			dir = cap->arg;	// TRUE for searching forward
    long		count = cap->count1;	// repeat count
    int			col;
    char_u		*p;
    int			len;
    int			stop = TRUE;

    if (c != NUL)	// normal search: remember args for repeat
    {
	if (!KeyStuffed)    // don't remember when redoing
	{
	    *lastc = c;
	    set_csearch_direction(dir);
	    set_csearch_until(t_cmd);
	    lastc_bytelen = (*mb_char2bytes)(c, lastc_bytes);
	    if (cap->ncharC1 != 0)
	    {
		lastc_bytelen += (*mb_char2bytes)(cap->ncharC1,
			lastc_bytes + lastc_bytelen);
		if (cap->ncharC2 != 0)
		    lastc_bytelen += (*mb_char2bytes)(cap->ncharC2,
			    lastc_bytes + lastc_bytelen);
	    }
	}
    }
    else		// repeat previous search
    {
	if (*lastc == NUL && lastc_bytelen == 1)
	    return FAIL;
	if (dir)	// repeat in opposite direction
	    dir = -lastcdir;
	else
	    dir = lastcdir;
	t_cmd = last_t_cmd;
	c = *lastc;
	// For multi-byte re-use last lastc_bytes[] and lastc_bytelen.

	// Force a move of at least one char, so ";" and "," will move the
	// cursor, even if the cursor is right in front of char we are looking
	// at.
	if (vim_strchr(p_cpo, CPO_SCOLON) == NULL && count == 1 && t_cmd)
	    stop = FALSE;
    }

    if (dir == BACKWARD)
	cap->oap->inclusive = FALSE;
    else
	cap->oap->inclusive = TRUE;

    p = ml_get_curline();
    col = curwin->w_cursor.col;
    len = (int)STRLEN(p);

    while (count--)
    {
	if (has_mbyte)
	{
	    for (;;)
	    {
		if (dir > 0)
		{
		    col += (*mb_ptr2len)(p + col);
		    if (col >= len)
			return FAIL;
		}
		else
		{
		    if (col == 0)
			return FAIL;
		    col -= (*mb_head_off)(p, p + col - 1) + 1;
		}
		if (lastc_bytelen == 1)
		{
		    if (p[col] == c && stop)
			break;
		}
		else if (STRNCMP(p + col, lastc_bytes, lastc_bytelen) == 0
								       && stop)
		    break;
		stop = TRUE;
	    }
	}
	else
	{
	    for (;;)
	    {
		if ((col += dir) < 0 || col >= len)
		    return FAIL;
		if (p[col] == c && stop)
		    break;
		stop = TRUE;
	    }
	}
    }

    if (t_cmd)
    {
	// backup to before the character (possibly double-byte)
	col -= dir;
	if (has_mbyte)
	{
	    if (dir < 0)
		// Landed on the search char which is lastc_bytelen long
		col += lastc_bytelen - 1;
	    else
		// To previous char, which may be multi-byte.
		col -= (*mb_head_off)(p, p + col);
	}
    }
    curwin->w_cursor.col = col;

    return OK;
}