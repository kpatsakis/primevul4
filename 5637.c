get_reg_contents(int regname, int flags)
{
    long	i;
    char_u	*retval;
    int		allocated;
    long	len;

    // Don't allow using an expression register inside an expression
    if (regname == '=')
    {
	if (flags & GREG_NO_EXPR)
	    return NULL;
	if (flags & GREG_EXPR_SRC)
	    return getreg_wrap_one_line(get_expr_line_src(), flags);
	return getreg_wrap_one_line(get_expr_line(), flags);
    }

    if (regname == '@')	    // "@@" is used for unnamed register
	regname = '"';

    // check for valid regname
    if (regname != NUL && !valid_yank_reg(regname, FALSE))
	return NULL;

# ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
# endif

    if (get_spec_reg(regname, &retval, &allocated, FALSE))
    {
	if (retval == NULL)
	    return NULL;
	if (allocated)
	    return getreg_wrap_one_line(retval, flags);
	return getreg_wrap_one_line(vim_strsave(retval), flags);
    }

    get_yank_register(regname, FALSE);
    if (y_current->y_array == NULL)
	return NULL;

    if (flags & GREG_LIST)
    {
	list_T	*list = list_alloc();
	int	error = FALSE;

	if (list == NULL)
	    return NULL;
	for (i = 0; i < y_current->y_size; ++i)
	    if (list_append_string(list, y_current->y_array[i], -1) == FAIL)
		error = TRUE;
	if (error)
	{
	    list_free(list);
	    return NULL;
	}
	return (char_u *)list;
    }

    // Compute length of resulting string.
    len = 0;
    for (i = 0; i < y_current->y_size; ++i)
    {
	len += (long)STRLEN(y_current->y_array[i]);
	// Insert a newline between lines and after last line if
	// y_type is MLINE.
	if (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = alloc(len + 1);

    // Copy the lines of the yank register into the string.
    if (retval != NULL)
    {
	len = 0;
	for (i = 0; i < y_current->y_size; ++i)
	{
	    STRCPY(retval + len, y_current->y_array[i]);
	    len += (long)STRLEN(retval + len);

	    // Insert a NL between lines and after the last line if y_type is
	    // MLINE.
	    if (y_current->y_type == MLINE || i < y_current->y_size - 1)
		retval[len++] = '\n';
	}
	retval[len] = NUL;
    }

    return retval;
}