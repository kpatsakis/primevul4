get_expr_line(void)
{
    char_u	*expr_copy;
    char_u	*rv;
    static int	nested = 0;

    if (expr_line == NULL)
	return NULL;

    // Make a copy of the expression, because evaluating it may cause it to be
    // changed.
    expr_copy = vim_strsave(expr_line);
    if (expr_copy == NULL)
	return NULL;

    // When we are invoked recursively limit the evaluation to 10 levels.
    // Then return the string as-is.
    if (nested >= 10)
	return expr_copy;

    ++nested;
    rv = eval_to_string_eap(expr_copy, TRUE, expr_eap);
    --nested;
    vim_free(expr_copy);
    return rv;
}