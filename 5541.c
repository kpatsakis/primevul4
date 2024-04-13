is_zero_width(char_u *pattern, int move, pos_T *cur, int direction)
{
    regmmatch_T	regmatch;
    int		nmatched = 0;
    int		result = -1;
    pos_T	pos;
    int		called_emsg_before = called_emsg;
    int		flag = 0;

    if (pattern == NULL)
	pattern = spats[last_idx].pat;

    if (search_regcomp(pattern, RE_SEARCH, RE_SEARCH,
					      SEARCH_KEEP, &regmatch) == FAIL)
	return -1;

    // init startcol correctly
    regmatch.startpos[0].col = -1;
    // move to match
    if (move)
    {
	CLEAR_POS(&pos);
    }
    else
    {
	pos = *cur;
	// accept a match at the cursor position
	flag = SEARCH_START;
    }

    if (searchit(curwin, curbuf, &pos, NULL, direction, pattern, 1,
				  SEARCH_KEEP + flag, RE_SEARCH, NULL) != FAIL)
    {
	// Zero-width pattern should match somewhere, then we can check if
	// start and end are in the same position.
	do
	{
	    regmatch.startpos[0].col++;
	    nmatched = vim_regexec_multi(&regmatch, curwin, curbuf,
			       pos.lnum, regmatch.startpos[0].col, NULL, NULL);
	    if (nmatched != 0)
		break;
	} while (regmatch.regprog != NULL
		&& direction == FORWARD ? regmatch.startpos[0].col < pos.col
				      : regmatch.startpos[0].col > pos.col);

	if (called_emsg == called_emsg_before)
	{
	    result = (nmatched != 0
		&& regmatch.startpos[0].lnum == regmatch.endpos[0].lnum
		&& regmatch.startpos[0].col == regmatch.endpos[0].col);
	}
    }

    vim_regfree(regmatch.regprog);
    return result;
}