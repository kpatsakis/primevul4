fuzzy_match_recursive(
	char_u		*fuzpat,
	char_u		*str,
	int_u		strIdx,
	int		*outScore,
	char_u		*strBegin,
	int		strLen,
	int_u		*srcMatches,
	int_u		*matches,
	int		maxMatches,
	int		nextMatch,
	int		*recursionCount)
{
    // Recursion params
    int		recursiveMatch = FALSE;
    int_u	bestRecursiveMatches[MAX_FUZZY_MATCHES];
    int		bestRecursiveScore = 0;
    int		first_match;
    int		matched;

    // Count recursions
    ++*recursionCount;
    if (*recursionCount >= FUZZY_MATCH_RECURSION_LIMIT)
	return 0;

    // Detect end of strings
    if (*fuzpat == NUL || *str == NUL)
	return 0;

    // Loop through fuzpat and str looking for a match
    first_match = TRUE;
    while (*fuzpat != NUL && *str != NUL)
    {
	int	c1;
	int	c2;

	c1 = PTR2CHAR(fuzpat);
	c2 = PTR2CHAR(str);

	// Found match
	if (vim_tolower(c1) == vim_tolower(c2))
	{
	    int_u	recursiveMatches[MAX_FUZZY_MATCHES];
	    int		recursiveScore = 0;
	    char_u	*next_char;

	    // Supplied matches buffer was too short
	    if (nextMatch >= maxMatches)
		return 0;

	    // "Copy-on-Write" srcMatches into matches
	    if (first_match && srcMatches)
	    {
		memcpy(matches, srcMatches, nextMatch * sizeof(srcMatches[0]));
		first_match = FALSE;
	    }

	    // Recursive call that "skips" this match
	    if (has_mbyte)
		next_char = str + (*mb_ptr2len)(str);
	    else
		next_char = str + 1;
	    if (fuzzy_match_recursive(fuzpat, next_char, strIdx + 1,
			&recursiveScore, strBegin, strLen, matches,
			recursiveMatches,
			ARRAY_LENGTH(recursiveMatches),
			nextMatch, recursionCount))
	    {
		// Pick best recursive score
		if (!recursiveMatch || recursiveScore > bestRecursiveScore)
		{
		    memcpy(bestRecursiveMatches, recursiveMatches,
			    MAX_FUZZY_MATCHES * sizeof(recursiveMatches[0]));
		    bestRecursiveScore = recursiveScore;
		}
		recursiveMatch = TRUE;
	    }

	    // Advance
	    matches[nextMatch++] = strIdx;
	    if (has_mbyte)
		MB_PTR_ADV(fuzpat);
	    else
		++fuzpat;
	}
	if (has_mbyte)
	    MB_PTR_ADV(str);
	else
	    ++str;
	strIdx++;
    }

    // Determine if full fuzpat was matched
    matched = *fuzpat == NUL ? TRUE : FALSE;

    // Calculate score
    if (matched)
	*outScore = fuzzy_match_compute_score(strBegin, strLen, matches,
		nextMatch);

    // Return best result
    if (recursiveMatch && (!matched || bestRecursiveScore > *outScore))
    {
	// Recursive score is better than "this"
	memcpy(matches, bestRecursiveMatches, maxMatches * sizeof(matches[0]));
	*outScore = bestRecursiveScore;
	return nextMatch;
    }
    else if (matched)
	return nextMatch;	// "this" score is better than recursive

    return 0;		// no match
}