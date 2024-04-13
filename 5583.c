fuzzy_match_str(char_u *str, char_u *pat)
{
    int		score = 0;
    int_u	matchpos[MAX_FUZZY_MATCHES];

    if (str == NULL || pat == NULL)
	return 0;

    fuzzy_match(str, pat, TRUE, &score, matchpos,
				sizeof(matchpos) / sizeof(matchpos[0]));

    return score;
}