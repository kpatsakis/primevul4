search_regcomp(
    char_u	*pat,
    int		pat_save,
    int		pat_use,
    int		options,
    regmmatch_T	*regmatch)	// return: pattern and ignore-case flag
{
    int		magic;
    int		i;

    rc_did_emsg = FALSE;
    magic = magic_isset();

    /*
     * If no pattern given, use a previously defined pattern.
     */
    if (pat == NULL || *pat == NUL)
    {
	if (pat_use == RE_LAST)
	    i = last_idx;
	else
	    i = pat_use;
	if (spats[i].pat == NULL)	// pattern was never defined
	{
	    if (pat_use == RE_SUBST)
		emsg(_(e_no_previous_substitute_regular_expression));
	    else
		emsg(_(e_no_previous_regular_expression));
	    rc_did_emsg = TRUE;
	    return FAIL;
	}
	pat = spats[i].pat;
	magic = spats[i].magic;
	no_smartcase = spats[i].no_scs;
    }
    else if (options & SEARCH_HIS)	// put new pattern in history
	add_to_history(HIST_SEARCH, pat, TRUE, NUL);

    vim_free(mr_pattern);
#ifdef FEAT_RIGHTLEFT
    if (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	mr_pattern = reverse_text(pat);
    else
#endif
	mr_pattern = vim_strsave(pat);

    /*
     * Save the currently used pattern in the appropriate place,
     * unless the pattern should not be remembered.
     */
    if (!(options & SEARCH_KEEP)
			       && (cmdmod.cmod_flags & CMOD_KEEPPATTERNS) == 0)
    {
	// search or global command
	if (pat_save == RE_SEARCH || pat_save == RE_BOTH)
	    save_re_pat(RE_SEARCH, pat, magic);
	// substitute or global command
	if (pat_save == RE_SUBST || pat_save == RE_BOTH)
	    save_re_pat(RE_SUBST, pat, magic);
    }

    regmatch->rmm_ic = ignorecase(pat);
    regmatch->rmm_maxcol = 0;
    regmatch->regprog = vim_regcomp(pat, magic ? RE_MAGIC : 0);
    if (regmatch->regprog == NULL)
	return FAIL;
    return OK;
}