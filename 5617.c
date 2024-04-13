write_reg_contents_lst(
    int		name,
    char_u	**strings,
    int		maxlen UNUSED,
    int		must_append,
    int		yank_type,
    long	block_len)
{
    yankreg_T  *old_y_previous, *old_y_current;

    if (name == '/' || name == '=')
    {
	char_u	*s;

	if (strings[0] == NULL)
	    s = (char_u *)"";
	else if (strings[1] != NULL)
	{
	    emsg(_(e_search_pattern_and_expression_register_may_not_contain_two_or_more_lines));
	    return;
	}
	else
	    s = strings[0];
	write_reg_contents_ex(name, s, -1, must_append, yank_type, block_len);
	return;
    }

    if (name == '_')	    // black hole: nothing to do
	return;

    if (init_write_reg(name, &old_y_previous, &old_y_current, must_append,
		&yank_type) == FAIL)
	return;

    str_to_reg(y_current, yank_type, (char_u *)strings, -1, block_len, TRUE);

    finish_write_reg(name, old_y_previous, old_y_current);
}