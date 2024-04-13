get_expr_line_src(void)
{
    if (expr_line == NULL)
	return NULL;
    return vim_strsave(expr_line);
}