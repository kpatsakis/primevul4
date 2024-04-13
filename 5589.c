f_searchcount(typval_T *argvars, typval_T *rettv)
{
    pos_T		pos = curwin->w_cursor;
    char_u		*pattern = NULL;
    int			maxcount = SEARCH_STAT_DEF_MAX_COUNT;
    long		timeout = SEARCH_STAT_DEF_TIMEOUT;
    int			recompute = TRUE;
    searchstat_T	stat;

    if (rettv_dict_alloc(rettv) == FAIL)
	return;

    if (in_vim9script() && check_for_opt_dict_arg(argvars, 0) == FAIL)
	return;

    if (shortmess(SHM_SEARCHCOUNT))	// 'shortmess' contains 'S' flag
	recompute = TRUE;

    if (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T		*dict;
	dictitem_T	*di;
	listitem_T	*li;
	int		error = FALSE;

	if (argvars[0].v_type != VAR_DICT || argvars[0].vval.v_dict == NULL)
	{
	    emsg(_(e_dictionary_required));
	    return;
	}
	dict = argvars[0].vval.v_dict;
	di = dict_find(dict, (char_u *)"timeout", -1);
	if (di != NULL)
	{
	    timeout = (long)tv_get_number_chk(&di->di_tv, &error);
	    if (error)
		return;
	}
	di = dict_find(dict, (char_u *)"maxcount", -1);
	if (di != NULL)
	{
	    maxcount = (int)tv_get_number_chk(&di->di_tv, &error);
	    if (error)
		return;
	}
	recompute = dict_get_bool(dict, (char_u *)"recompute", recompute);
	di = dict_find(dict, (char_u *)"pattern", -1);
	if (di != NULL)
	{
	    pattern = tv_get_string_chk(&di->di_tv);
	    if (pattern == NULL)
		return;
	}
	di = dict_find(dict, (char_u *)"pos", -1);
	if (di != NULL)
	{
	    if (di->di_tv.v_type != VAR_LIST)
	    {
		semsg(_(e_invalid_argument_str), "pos");
		return;
	    }
	    if (list_len(di->di_tv.vval.v_list) != 3)
	    {
		semsg(_(e_invalid_argument_str), "List format should be [lnum, col, off]");
		return;
	    }
	    li = list_find(di->di_tv.vval.v_list, 0L);
	    if (li != NULL)
	    {
		pos.lnum = tv_get_number_chk(&li->li_tv, &error);
		if (error)
		    return;
	    }
	    li = list_find(di->di_tv.vval.v_list, 1L);
	    if (li != NULL)
	    {
		pos.col = tv_get_number_chk(&li->li_tv, &error) - 1;
		if (error)
		    return;
	    }
	    li = list_find(di->di_tv.vval.v_list, 2L);
	    if (li != NULL)
	    {
		pos.coladd = tv_get_number_chk(&li->li_tv, &error);
		if (error)
		    return;
	    }
	}
    }

    save_last_search_pattern();
#ifdef FEAT_SEARCH_EXTRA
    save_incsearch_state();
#endif
    if (pattern != NULL)
    {
	if (*pattern == NUL)
	    goto the_end;
	vim_free(spats[last_idx].pat);
	spats[last_idx].pat = vim_strsave(pattern);
    }
    if (spats[last_idx].pat == NULL || *spats[last_idx].pat == NUL)
	goto the_end;	// the previous pattern was never defined

    update_search_stat(0, &pos, &pos, &stat, recompute, maxcount, timeout);

    dict_add_number(rettv->vval.v_dict, "current", stat.cur);
    dict_add_number(rettv->vval.v_dict, "total", stat.cnt);
    dict_add_number(rettv->vval.v_dict, "exact_match", stat.exact_match);
    dict_add_number(rettv->vval.v_dict, "incomplete", stat.incomplete);
    dict_add_number(rettv->vval.v_dict, "maxcount", stat.last_maxcount);

the_end:
    restore_last_search_pattern();
#ifdef FEAT_SEARCH_EXTRA
    restore_incsearch_state();
#endif
}