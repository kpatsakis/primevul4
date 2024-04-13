current_search(
    long	count,
    int		forward)	// TRUE for forward, FALSE for backward
{
    pos_T	start_pos;	// start position of the pattern match
    pos_T	end_pos;	// end position of the pattern match
    pos_T	orig_pos;	// position of the cursor at beginning
    pos_T	pos;		// position after the pattern
    int		i;
    int		dir;
    int		result;		// result of various function calls
    char_u	old_p_ws = p_ws;
    int		flags = 0;
    pos_T	save_VIsual = VIsual;
    int		zero_width;
    int		skip_first_backward;

    // Correct cursor when 'selection' is exclusive
    if (VIsual_active && *p_sel == 'e' && LT_POS(VIsual, curwin->w_cursor))
	dec_cursor();

    // When searching forward and the cursor is at the start of the Visual
    // area, skip the first search backward, otherwise it doesn't move.
    skip_first_backward = forward && VIsual_active
					   && LT_POS(curwin->w_cursor, VIsual);

    orig_pos = pos = curwin->w_cursor;
    if (VIsual_active)
    {
	if (forward)
	    incl(&pos);
	else
	    decl(&pos);
    }

    // Is the pattern is zero-width?, this time, don't care about the direction
    zero_width = is_zero_width(spats[last_idx].pat, TRUE, &curwin->w_cursor,
								      FORWARD);
    if (zero_width == -1)
	return FAIL;  // pattern not found

    /*
     * The trick is to first search backwards and then search forward again,
     * so that a match at the current cursor position will be correctly
     * captured.  When "forward" is false do it the other way around.
     */
    for (i = 0; i < 2; i++)
    {
	if (forward)
	{
	    if (i == 0 && skip_first_backward)
		continue;
	    dir = i;
	}
	else
	    dir = !i;

	flags = 0;
	if (!dir && !zero_width)
	    flags = SEARCH_END;
	end_pos = pos;

	// wrapping should not occur in the first round
	if (i == 0)
	    p_ws = FALSE;

	result = searchit(curwin, curbuf, &pos, &end_pos,
		(dir ? FORWARD : BACKWARD),
		spats[last_idx].pat, (long) (i ? count : 1),
		SEARCH_KEEP | flags, RE_SEARCH, NULL);

	p_ws = old_p_ws;

	// First search may fail, but then start searching from the
	// beginning of the file (cursor might be on the search match)
	// except when Visual mode is active, so that extending the visual
	// selection works.
	if (i == 1 && !result) // not found, abort
	{
	    curwin->w_cursor = orig_pos;
	    if (VIsual_active)
		VIsual = save_VIsual;
	    return FAIL;
	}
	else if (i == 0 && !result)
	{
	    if (forward)
	    {
		// try again from start of buffer
		CLEAR_POS(&pos);
	    }
	    else
	    {
		// try again from end of buffer
		// searching backwards, so set pos to last line and col
		pos.lnum = curwin->w_buffer->b_ml.ml_line_count;
		pos.col  = (colnr_T)STRLEN(
				ml_get(curwin->w_buffer->b_ml.ml_line_count));
	    }
	}
    }

    start_pos = pos;

    if (!VIsual_active)
	VIsual = start_pos;

    // put the cursor after the match
    curwin->w_cursor = end_pos;
    if (LT_POS(VIsual, end_pos) && forward)
    {
	if (skip_first_backward)
	    // put the cursor on the start of the match
	    curwin->w_cursor = pos;
	else
	    // put the cursor on last character of match
	    dec_cursor();
    }
    else if (VIsual_active && LT_POS(curwin->w_cursor, VIsual) && forward)
	curwin->w_cursor = pos;   // put the cursor on the start of the match
    VIsual_active = TRUE;
    VIsual_mode = 'v';

    if (*p_sel == 'e')
    {
	// Correction for exclusive selection depends on the direction.
	if (forward && LTOREQ_POS(VIsual, curwin->w_cursor))
	    inc_cursor();
	else if (!forward && LTOREQ_POS(curwin->w_cursor, VIsual))
	    inc(&VIsual);
    }

#ifdef FEAT_FOLDING
    if (fdo_flags & FDO_SEARCH && KeyTyped)
	foldOpenCursor();
#endif

    may_start_select('c');
    setmouse();
#ifdef FEAT_CLIPBOARD
    // Make sure the clipboard gets updated.  Needed because start and
    // end are still the same, and the selection needs to be owned
    clip_star.vmode = NUL;
#endif
    redraw_curbuf_later(INVERTED);
    showmode();

    return OK;
}