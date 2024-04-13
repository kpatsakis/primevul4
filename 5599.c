execreg_line_continuation(char_u **lines, long *idx)
{
    garray_T	ga;
    long	i = *idx;
    char_u	*p;
    int		cmd_start;
    int		cmd_end = i;
    int		j;
    char_u	*str;

    ga_init2(&ga, sizeof(char_u), 400);

    // search backwards to find the first line of this command.
    // Any line not starting with \ or "\ is the start of the
    // command.
    while (--i > 0)
    {
	p = skipwhite(lines[i]);
	if (*p != '\\' && (p[0] != '"' || p[1] != '\\' || p[2] != ' '))
	    break;
    }
    cmd_start = i;

    // join all the lines
    ga_concat(&ga, lines[cmd_start]);
    for (j = cmd_start + 1; j <= cmd_end; j++)
    {
	p = skipwhite(lines[j]);
	if (*p == '\\')
	{
	    // Adjust the growsize to the current length to
	    // speed up concatenating many lines.
	    if (ga.ga_len > 400)
	    {
		if (ga.ga_len > 8000)
		    ga.ga_growsize = 8000;
		else
		    ga.ga_growsize = ga.ga_len;
	    }
	    ga_concat(&ga, p + 1);
	}
    }
    ga_append(&ga, NUL);
    str = vim_strsave(ga.ga_data);
    ga_clear(&ga);

    *idx = i;
    return str;
}