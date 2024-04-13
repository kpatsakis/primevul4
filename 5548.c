last_pat_prog(regmmatch_T *regmatch)
{
    if (spats[last_idx].pat == NULL)
    {
	regmatch->regprog = NULL;
	return;
    }
    ++emsg_off;		// So it doesn't beep if bad expr
    (void)search_regcomp((char_u *)"", 0, last_idx, SEARCH_KEEP, regmatch);
    --emsg_off;
}