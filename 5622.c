get_expr_register(void)
{
    char_u	*new_line;

    new_line = getcmdline('=', 0L, 0, 0);
    if (new_line == NULL)
	return NUL;
    if (*new_line == NUL)	// use previous line
	vim_free(new_line);
    else
	set_expr_line(new_line, NULL);
    return '=';
}