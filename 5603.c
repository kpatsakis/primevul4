insert_reg(
    int		regname,
    int		literally_arg)	// insert literally, not as if typed
{
    long	i;
    int		retval = OK;
    char_u	*arg;
    int		allocated;
    int		literally = literally_arg;

    // It is possible to get into an endless loop by having CTRL-R a in
    // register a and then, in insert mode, doing CTRL-R a.
    // If you hit CTRL-C, the loop will be broken here.
    ui_breakcheck();
    if (got_int)
	return FAIL;

    // check for valid regname
    if (regname != NUL && !valid_yank_reg(regname, FALSE))
	return FAIL;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    if (regname == '.')			// insert last inserted text
	retval = stuff_inserted(NUL, 1L, TRUE);
    else if (get_spec_reg(regname, &arg, &allocated, TRUE))
    {
	if (arg == NULL)
	    return FAIL;
	stuffescaped(arg, literally);
	if (allocated)
	    vim_free(arg);
    }
    else				// name or number register
    {
	if (get_yank_register(regname, FALSE))
	    literally = TRUE;
	if (y_current->y_array == NULL)
	    retval = FAIL;
	else
	{
	    for (i = 0; i < y_current->y_size; ++i)
	    {
		if (regname == '-')
		{
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(regname);
		    do_put(regname, NULL, BACKWARD, 1L, PUT_CURSEND);
		}
		else
		    stuffescaped(y_current->y_array[i], literally);
		// Insert a newline between lines and after last line if
		// y_type is MLINE.
		if (y_current->y_type == MLINE || i < y_current->y_size - 1)
		    stuffcharReadbuff('\n');
	    }
	}
    }

    return retval;
}