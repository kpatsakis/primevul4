do_execreg(
    int	    regname,
    int	    colon,		// insert ':' before each line
    int	    addcr,		// always add '\n' to end of line
    int	    silent)		// set "silent" flag in typeahead buffer
{
    long	i;
    char_u	*p;
    int		retval = OK;
    int		remap;

    // repeat previous one
    if (regname == '@')
    {
	if (execreg_lastc == NUL)
	{
	    emsg(_(e_no_previously_used_register));
	    return FAIL;
	}
	regname = execreg_lastc;
    }
    // check for valid regname
    if (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
    {
	emsg_invreg(regname);
	return FAIL;
    }
    execreg_lastc = regname;

#ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#endif

    // black hole: don't stuff anything
    if (regname == '_')
	return OK;

    // use last command line
    if (regname == ':')
    {
	if (last_cmdline == NULL)
	{
	    emsg(_(e_no_previous_command_line));
	    return FAIL;
	}
	// don't keep the cmdline containing @:
	VIM_CLEAR(new_last_cmdline);
	// Escape all control characters with a CTRL-V
	p = vim_strsave_escaped_ext(last_cmdline,
		    (char_u *)"\001\002\003\004\005\006\007"
			  "\010\011\012\013\014\015\016\017"
			  "\020\021\022\023\024\025\026\027"
			  "\030\031\032\033\034\035\036\037",
		    Ctrl_V, FALSE);
	if (p != NULL)
	{
	    // When in Visual mode "'<,'>" will be prepended to the command.
	    // Remove it when it's already there.
	    if (VIsual_active && STRNCMP(p, "'<,'>", 5) == 0)
		retval = put_in_typebuf(p + 5, TRUE, TRUE, silent);
	    else
		retval = put_in_typebuf(p, TRUE, TRUE, silent);
	}
	vim_free(p);
    }
#ifdef FEAT_EVAL
    else if (regname == '=')
    {
	p = get_expr_line();
	if (p == NULL)
	    return FAIL;
	retval = put_in_typebuf(p, TRUE, colon, silent);
	vim_free(p);
    }
#endif
    else if (regname == '.')		// use last inserted text
    {
	p = get_last_insert_save();
	if (p == NULL)
	{
	    emsg(_(e_no_inserted_text_yet));
	    return FAIL;
	}
	retval = put_in_typebuf(p, FALSE, colon, silent);
	vim_free(p);
    }
    else
    {
	get_yank_register(regname, FALSE);
	if (y_current->y_array == NULL)
	    return FAIL;

	// Disallow remapping for ":@r".
	remap = colon ? REMAP_NONE : REMAP_YES;

	// Insert lines into typeahead buffer, from last one to first one.
	put_reedit_in_typebuf(silent);
	for (i = y_current->y_size; --i >= 0; )
	{
	    char_u *escaped;
	    char_u *str;
	    int	    free_str = FALSE;

	    // insert NL between lines and after last line if type is MLINE
	    if (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		if (ins_typebuf((char_u *)"\n", remap, 0, TRUE, silent) == FAIL)
		    return FAIL;
	    }

	    // Handle line-continuation for :@<register>
	    str = y_current->y_array[i];
	    if (colon && i > 0)
	    {
		p = skipwhite(str);
		if (*p == '\\' || (p[0] == '"' && p[1] == '\\' && p[2] == ' '))
		{
		    str = execreg_line_continuation(y_current->y_array, &i);
		    if (str == NULL)
			return FAIL;
		    free_str = TRUE;
		}
	    }
	    escaped = vim_strsave_escape_csi(str);
	    if (free_str)
		vim_free(str);
	    if (escaped == NULL)
		return FAIL;
	    retval = ins_typebuf(escaped, remap, 0, TRUE, silent);
	    vim_free(escaped);
	    if (retval == FAIL)
		return FAIL;
	    if (colon && ins_typebuf((char_u *)":", remap, 0, TRUE, silent)
								      == FAIL)
		return FAIL;
	}
	reg_executing = regname == 0 ? '"' : regname; // disable "q" command
    }
    return retval;
}