do_fuzzymatch(typval_T *argvars, typval_T *rettv, int retmatchpos)
{
    callback_T	cb;
    char_u	*key = NULL;
    int		ret;
    int		matchseq = FALSE;
    long	max_matches = 0;

    if (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	return;

    CLEAR_POINTER(&cb);

    // validate and get the arguments
    if (argvars[0].v_type != VAR_LIST || argvars[0].vval.v_list == NULL)
    {
	semsg(_(e_argument_of_str_must_be_list),
			     retmatchpos ? "matchfuzzypos()" : "matchfuzzy()");
	return;
    }
    if (argvars[1].v_type != VAR_STRING
	    || argvars[1].vval.v_string == NULL)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&argvars[1]));
	return;
    }

    if (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T		*d;
	dictitem_T	*di;

	if (argvars[2].v_type != VAR_DICT || argvars[2].vval.v_dict == NULL)
	{
	    emsg(_(e_dictionary_required));
	    return;
	}

	// To search a dict, either a callback function or a key can be
	// specified.
	d = argvars[2].vval.v_dict;
	if ((di = dict_find(d, (char_u *)"key", -1)) != NULL)
	{
	    if (di->di_tv.v_type != VAR_STRING
		    || di->di_tv.vval.v_string == NULL
		    || *di->di_tv.vval.v_string == NUL)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
		return;
	    }
	    key = tv_get_string(&di->di_tv);
	}
	else if ((di = dict_find(d, (char_u *)"text_cb", -1)) != NULL)
	{
	    cb = get_callback(&di->di_tv);
	    if (cb.cb_name == NULL)
	    {
		semsg(_(e_invalid_value_for_argument_str), "text_cb");
		return;
	    }
	}
	else if ((di = dict_find(d, (char_u *)"limit", -1)) != NULL)
	{
	    if (di->di_tv.v_type != VAR_NUMBER)
	    {
		semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
		return;
	    }
	    max_matches = (long)tv_get_number_chk(&di->di_tv, NULL);
	}

	if (dict_has_key(d, "matchseq"))
	    matchseq = TRUE;
    }

    // get the fuzzy matches
    ret = rettv_list_alloc(rettv);
    if (ret != OK)
	goto done;
    if (retmatchpos)
    {
	list_T	*l;

	// For matchfuzzypos(), a list with three items are returned. First
	// item is a list of matching strings, the second item is a list of
	// lists with matching positions within each string and the third item
	// is the list of scores of the matches.
	l = list_alloc();
	if (l == NULL)
	    goto done;
	if (list_append_list(rettv->vval.v_list, l) == FAIL)
	    goto done;
	l = list_alloc();
	if (l == NULL)
	    goto done;
	if (list_append_list(rettv->vval.v_list, l) == FAIL)
	    goto done;
	l = list_alloc();
	if (l == NULL)
	    goto done;
	if (list_append_list(rettv->vval.v_list, l) == FAIL)
	    goto done;
    }

    fuzzy_match_in_list(argvars[0].vval.v_list, tv_get_string(&argvars[1]),
	    matchseq, key, &cb, retmatchpos, rettv->vval.v_list, max_matches);

done:
    free_callback(&cb);
}