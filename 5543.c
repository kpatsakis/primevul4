fuzzy_match_compute_score(
	char_u		*str,
	int		strSz,
	int_u		*matches,
	int		numMatches)
{
    int		score;
    int		penalty;
    int		unmatched;
    int		i;
    char_u	*p = str;
    int_u	sidx = 0;

    // Initialize score
    score = 100;

    // Apply leading letter penalty
    penalty = LEADING_LETTER_PENALTY * matches[0];
    if (penalty < MAX_LEADING_LETTER_PENALTY)
	penalty = MAX_LEADING_LETTER_PENALTY;
    score += penalty;

    // Apply unmatched penalty
    unmatched = strSz - numMatches;
    score += UNMATCHED_LETTER_PENALTY * unmatched;

    // Apply ordering bonuses
    for (i = 0; i < numMatches; ++i)
    {
	int_u	currIdx = matches[i];

	if (i > 0)
	{
	    int_u	prevIdx = matches[i - 1];

	    // Sequential
	    if (currIdx == (prevIdx + 1))
		score += SEQUENTIAL_BONUS;
	    else
		score += GAP_PENALTY * (currIdx - prevIdx);
	}

	// Check for bonuses based on neighbor character value
	if (currIdx > 0)
	{
	    // Camel case
	    int	neighbor = ' ';
	    int	curr;

	    if (has_mbyte)
	    {
		while (sidx < currIdx)
		{
		    neighbor = (*mb_ptr2char)(p);
		    MB_PTR_ADV(p);
		    sidx++;
		}
		curr = (*mb_ptr2char)(p);
	    }
	    else
	    {
		neighbor = str[currIdx - 1];
		curr = str[currIdx];
	    }

	    if (vim_islower(neighbor) && vim_isupper(curr))
		score += CAMEL_BONUS;

	    // Bonus if the match follows a separator character
	    if (neighbor == '/' || neighbor == '\\')
		score += PATH_SEPARATOR_BONUS;
	    else if (neighbor == ' ' || neighbor == '_')
		score += WORD_SEPARATOR_BONUS;
	}
	else
	{
	    // First letter
	    score += FIRST_LETTER_BONUS;
	}
    }
    return score;
}