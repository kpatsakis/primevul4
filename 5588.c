update_search_stat(
    int			dirc,
    pos_T		*pos,
    pos_T		*cursor_pos,
    searchstat_T	*stat,
    int			recompute,
    int			maxcount,
    long		timeout UNUSED)
{
    int		    save_ws = p_ws;
    int		    wraparound = FALSE;
    pos_T	    p = (*pos);
    static pos_T    lastpos = {0, 0, 0};
    static int	    cur = 0;
    static int	    cnt = 0;
    static int	    exact_match = FALSE;
    static int	    incomplete = 0;
    static int	    last_maxcount = SEARCH_STAT_DEF_MAX_COUNT;
    static int	    chgtick = 0;
    static char_u   *lastpat = NULL;
    static buf_T    *lbuf = NULL;
#ifdef FEAT_RELTIME
    proftime_T  start;
#endif

    vim_memset(stat, 0, sizeof(searchstat_T));

    if (dirc == 0 && !recompute && !EMPTY_POS(lastpos))
    {
	stat->cur = cur;
	stat->cnt = cnt;
	stat->exact_match = exact_match;
	stat->incomplete = incomplete;
	stat->last_maxcount = last_maxcount;
	return;
    }
    last_maxcount = maxcount;

    wraparound = ((dirc == '?' && LT_POS(lastpos, p))
	       || (dirc == '/' && LT_POS(p, lastpos)));

    // If anything relevant changed the count has to be recomputed.
    // MB_STRNICMP ignores case, but we should not ignore case.
    // Unfortunately, there is no MB_STRNICMP function.
    // XXX: above comment should be "no MB_STRCMP function" ?
    if (!(chgtick == CHANGEDTICK(curbuf)
	&& MB_STRNICMP(lastpat, spats[last_idx].pat, STRLEN(lastpat)) == 0
	&& STRLEN(lastpat) == STRLEN(spats[last_idx].pat)
	&& EQUAL_POS(lastpos, *cursor_pos)
	&& lbuf == curbuf) || wraparound || cur < 0
	    || (maxcount > 0 && cur > maxcount) || recompute)
    {
	cur = 0;
	cnt = 0;
	exact_match = FALSE;
	incomplete = 0;
	CLEAR_POS(&lastpos);
	lbuf = curbuf;
    }

    if (EQUAL_POS(lastpos, *cursor_pos) && !wraparound
		&& (dirc == 0 || dirc == '/' ? cur < cnt : cur > 0))
	cur += dirc == 0 ? 0 : dirc == '/' ? 1 : -1;
    else
    {
	int	done_search = FALSE;
	pos_T	endpos = {0, 0, 0};

	p_ws = FALSE;
#ifdef FEAT_RELTIME
	if (timeout > 0)
	    profile_setlimit(timeout, &start);
#endif
	while (!got_int && searchit(curwin, curbuf, &lastpos, &endpos,
			 FORWARD, NULL, 1, SEARCH_KEEP, RE_LAST, NULL) != FAIL)
	{
	    done_search = TRUE;
#ifdef FEAT_RELTIME
	    // Stop after passing the time limit.
	    if (timeout > 0 && profile_passed_limit(&start))
	    {
		incomplete = 1;
		break;
	    }
#endif
	    cnt++;
	    if (LTOREQ_POS(lastpos, p))
	    {
		cur = cnt;
		if (LT_POS(p, endpos))
		    exact_match = TRUE;
	    }
	    fast_breakcheck();
	    if (maxcount > 0 && cnt > maxcount)
	    {
		incomplete = 2;    // max count exceeded
		break;
	    }
	}
	if (got_int)
	    cur = -1; // abort
	if (done_search)
	{
	    vim_free(lastpat);
	    lastpat = vim_strsave(spats[last_idx].pat);
	    chgtick = CHANGEDTICK(curbuf);
	    lbuf = curbuf;
	    lastpos = p;
	}
    }
    stat->cur = cur;
    stat->cnt = cnt;
    stat->exact_match = exact_match;
    stat->incomplete = incomplete;
    stat->last_maxcount = last_maxcount;
    p_ws = save_ws;
}