str_to_reg(
    yankreg_T	*y_ptr,		// pointer to yank register
    int		yank_type,	// MCHAR, MLINE, MBLOCK, MAUTO
    char_u	*str,		// string to put in register
    long	len,		// length of string
    long	blocklen,	// width of Visual block
    int		str_list)	// TRUE if str is char_u **
{
    int		type;			// MCHAR, MLINE or MBLOCK
    int		lnum;
    long	start;
    long	i;
    int		extra;
    int		newlines;		// number of lines added
    int		extraline = 0;		// extra line at the end
    int		append = FALSE;		// append to last line in register
    char_u	*s;
    char_u	**ss;
    char_u	**pp;
    long	maxlen;

    if (y_ptr->y_array == NULL)		// NULL means empty register
	y_ptr->y_size = 0;

    if (yank_type == MAUTO)
	type = ((str_list || (len > 0 && (str[len - 1] == NL
					    || str[len - 1] == CAR)))
							     ? MLINE : MCHAR);
    else
	type = yank_type;

    // Count the number of lines within the string
    newlines = 0;
    if (str_list)
    {
	for (ss = (char_u **) str; *ss != NULL; ++ss)
	    ++newlines;
    }
    else
    {
	for (i = 0; i < len; i++)
	    if (str[i] == '\n')
		++newlines;
	if (type == MCHAR || len == 0 || str[len - 1] != '\n')
	{
	    extraline = 1;
	    ++newlines;	// count extra newline at the end
	}
	if (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
	{
	    append = TRUE;
	    --newlines;	// uncount newline when appending first line
	}
    }

    // Without any lines make the register empty.
    if (y_ptr->y_size + newlines == 0)
    {
	VIM_CLEAR(y_ptr->y_array);
	return;
    }

    // Allocate an array to hold the pointers to the new register lines.
    // If the register was not empty, move the existing lines to the new array.
    pp = lalloc_clear((y_ptr->y_size + newlines) * sizeof(char_u *), TRUE);
    if (pp == NULL)	// out of memory
	return;
    for (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;
    maxlen = 0;

    // Find the end of each line and save it into the array.
    if (str_list)
    {
	for (ss = (char_u **) str; *ss != NULL; ++ss, ++lnum)
	{
	    pp[lnum] = vim_strsave(*ss);
	    if (type == MBLOCK)
	    {
		int charlen = mb_string2cells(*ss, -1);

		if (charlen > maxlen)
		    maxlen = charlen;
	    }
	}
    }
    else
    {
	for (start = 0; start < len + extraline; start += i + 1)
	{
	    int charlen = 0;

	    for (i = start; i < len; ++i)	// find the end of the line
	    {
		if (str[i] == '\n')
		    break;
		if (type == MBLOCK)
		    charlen += mb_ptr2cells_len(str + i, len - i);
	    }
	    i -= start;			// i is now length of line
	    if (charlen > maxlen)
		maxlen = charlen;
	    if (append)
	    {
		--lnum;
		extra = (int)STRLEN(y_ptr->y_array[lnum]);
	    }
	    else
		extra = 0;
	    s = alloc(i + extra + 1);
	    if (s == NULL)
		break;
	    if (extra)
		mch_memmove(s, y_ptr->y_array[lnum], (size_t)extra);
	    if (append)
		vim_free(y_ptr->y_array[lnum]);
	    if (i > 0)
		mch_memmove(s + extra, str + start, (size_t)i);
	    extra += i;
	    s[extra] = NUL;
	    y_ptr->y_array[lnum++] = s;
	    while (--extra >= 0)
	    {
		if (*s == NUL)
		    *s = '\n';	    // replace NUL with newline
		++s;
	    }
	    append = FALSE;		    // only first line is appended
	}
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
    if (type == MBLOCK)
	y_ptr->y_width = (blocklen < 0 ? maxlen - 1 : blocklen);
    else
	y_ptr->y_width = 0;
# ifdef FEAT_VIMINFO
    y_ptr->y_time_set = vim_time();
# endif
}