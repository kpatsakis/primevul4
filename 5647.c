write_reg_contents_ex(
    int		name,
    char_u	*str,
    int		maxlen,
    int		must_append,
    int		yank_type,
    long	block_len)
{
    yankreg_T	*old_y_previous, *old_y_current;
    long	len;

    if (maxlen >= 0)
	len = maxlen;
    else
	len = (long)STRLEN(str);

    // Special case: '/' search pattern
    if (name == '/')
    {
	set_last_search_pat(str, RE_SEARCH, TRUE, TRUE);
	return;
    }

    if (name == '#')
    {
	buf_T	*buf;

	if (VIM_ISDIGIT(*str))
	{
	    int	num = atoi((char *)str);

	    buf = buflist_findnr(num);
	    if (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist), (long)num);
	}
	else
	    buf = buflist_findnr(buflist_findpat(str, str + STRLEN(str),
							 TRUE, FALSE, FALSE));
	if (buf == NULL)
	    return;
	curwin->w_alt_fnum = buf->b_fnum;
	return;
    }

    if (name == '=')
    {
	char_u	    *p, *s;

	p = vim_strnsave(str, len);
	if (p == NULL)
	    return;
	if (must_append && expr_line != NULL)
	{
	    s = concat_str(expr_line, p);
	    vim_free(p);
	    p = s;
	}
	set_expr_line(p, NULL);
	return;
    }

    if (name == '_')	    // black hole: nothing to do
	return;

    if (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	return;

    str_to_reg(y_current, yank_type, str, len, block_len, FALSE);

    finish_write_reg(name, old_y_previous, old_y_current);
}