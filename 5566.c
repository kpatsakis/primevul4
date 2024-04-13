check_linecomment(char_u *line)
{
    char_u  *p;

    p = line;
    // skip Lispish one-line comments
    if (curbuf->b_p_lisp)
    {
	if (vim_strchr(p, ';') != NULL) // there may be comments
	{
	    int in_str = FALSE;	// inside of string

	    p = line;		// scan from start
	    while ((p = vim_strpbrk(p, (char_u *)"\";")) != NULL)
	    {
		if (*p == '"')
		{
		    if (in_str)
		    {
			if (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    else if (p == line || ((p - line) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		else if (!in_str && ((p - line) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(line, (colnr_T)(p - line)))
		    break;	// found!
		++p;
	    }
	}
	else
	    p = NULL;
    }
    else
	while ((p = vim_strchr(p, '/')) != NULL)
	{
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.  Only
	    // accept the position if it is not inside a string.
	    if (p[1] == '/' && (p == line || p[-1] != '*' || p[2] != '*')
			       && !is_pos_in_string(line, (colnr_T)(p - line)))
		break;
	    ++p;
	}

    if (p == NULL)
	return MAXCOL;
    return (int)(p - line);
}