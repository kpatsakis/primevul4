fuzzy_match(
	char_u		*str,
	char_u		*pat_arg,
	int		matchseq,
	int		*outScore,
	int_u		*matches,
	int		maxMatches)
{
    int		recursionCount = 0;
    int		len = MB_CHARLEN(str);
    char_u	*save_pat;
    char_u	*pat;
    char_u	*p;
    int		complete = FALSE;
    int		score = 0;
    int		numMatches = 0;
    int		matchCount;

    *outScore = 0;

    save_pat = vim_strsave(pat_arg);
    if (save_pat == NULL)
	return FALSE;
    pat = save_pat;
    p = pat;

    // Try matching each word in 'pat_arg' in 'str'
    while (TRUE)
    {
	if (matchseq)
	    complete = TRUE;
	else
	{
	    // Extract one word from the pattern (separated by space)
	    p = skipwhite(p);
	    if (*p == NUL)
		break;
	    pat = p;
	    while (*p != NUL && !VIM_ISWHITE(PTR2CHAR(p)))
	    {
		if (has_mbyte)
		    MB_PTR_ADV(p);
		else
		    ++p;
	    }
	    if (*p == NUL)		// processed all the words
		complete = TRUE;
	    *p = NUL;
	}

	score = 0;
	recursionCount = 0;
	matchCount = fuzzy_match_recursive(pat, str, 0, &score, str, len, NULL,
				matches + numMatches, maxMatches - numMatches,
				0, &recursionCount);
	if (matchCount == 0)
	{
	    numMatches = 0;
	    break;
	}

	// Accumulate the match score and the number of matches
	*outScore += score;
	numMatches += matchCount;

	if (complete)
	    break;

	// try matching the next word
	++p;
    }

    vim_free(save_pat);
    return numMatches != 0;
}