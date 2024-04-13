cmdline_search_stat(
    int		dirc,
    pos_T	*pos,
    pos_T	*cursor_pos,
    int		show_top_bot_msg,
    char_u	*msgbuf,
    int		recompute,
    int		maxcount,
    long	timeout)
{
    searchstat_T stat;

    update_search_stat(dirc, pos, cursor_pos, &stat, recompute, maxcount,
								      timeout);
    if (stat.cur > 0)
    {
	char	t[SEARCH_STAT_BUF_LEN];
	size_t	len;

#ifdef FEAT_RIGHTLEFT
	if (curwin->w_p_rl && *curwin->w_p_rlc == 's')
	{
	    if (stat.incomplete == 1)
		vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	    else if (stat.cnt > maxcount && stat.cur > maxcount)
		vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
							   maxcount, maxcount);
	    else if (stat.cnt > maxcount)
		vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/%d]",
							   maxcount, stat.cur);
	    else
		vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
							   stat.cnt, stat.cur);
	}
	else
#endif
	{
	    if (stat.incomplete == 1)
		vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[?/??]");
	    else if (stat.cnt > maxcount && stat.cur > maxcount)
		vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[>%d/>%d]",
							   maxcount, maxcount);
	    else if (stat.cnt > maxcount)
		vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/>%d]",
							   stat.cur, maxcount);
	    else
		vim_snprintf(t, SEARCH_STAT_BUF_LEN, "[%d/%d]",
							   stat.cur, stat.cnt);
	}

	len = STRLEN(t);
	if (show_top_bot_msg && len + 2 < SEARCH_STAT_BUF_LEN)
	{
	    mch_memmove(t + 2, t, len);
	    t[0] = 'W';
	    t[1] = ' ';
	    len += 2;
	}

	mch_memmove(msgbuf + STRLEN(msgbuf) - len, t, len);
	if (dirc == '?' && stat.cur == maxcount + 1)
	    stat.cur = -1;

	// keep the message even after redraw, but don't put in history
	msg_hist_off = TRUE;
	give_warning(msgbuf, FALSE);
	msg_hist_off = FALSE;
    }
}