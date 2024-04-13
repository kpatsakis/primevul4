search_for_exact_line(
    buf_T	*buf,
    pos_T	*pos,
    int		dir,
    char_u	*pat)
{
    linenr_T	start = 0;
    char_u	*ptr;
    char_u	*p;

    if (buf->b_ml.ml_line_count == 0)
	return FAIL;
    for (;;)
    {
	pos->lnum += dir;
	if (pos->lnum < 1)
	{
	    if (p_ws)
	    {
		pos->lnum = buf->b_ml.ml_line_count;
		if (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(top_bot_msg), TRUE);
	    }
	    else
	    {
		pos->lnum = 1;
		break;
	    }
	}
	else if (pos->lnum > buf->b_ml.ml_line_count)
	{
	    if (p_ws)
	    {
		pos->lnum = 1;
		if (!shortmess(SHM_SEARCH))
		    give_warning((char_u *)_(bot_top_msg), TRUE);
	    }
	    else
	    {
		pos->lnum = 1;
		break;
	    }
	}
	if (pos->lnum == start)
	    break;
	if (start == 0)
	    start = pos->lnum;
	ptr = ml_get_buf(buf, pos->lnum, FALSE);
	p = skipwhite(ptr);
	pos->col = (colnr_T) (p - ptr);

	// when adding lines the matching line may be empty but it is not
	// ignored because we are interested in the next line -- Acevedo
	if (compl_status_adding() && !compl_status_sol())
	{
	    if ((p_ic ? MB_STRICMP(p, pat) : STRCMP(p, pat)) == 0)
		return OK;
	}
	else if (*p != NUL)	// ignore empty lines
	{	// expanding lines or words
	    if ((p_ic ? MB_STRNICMP(p, pat, ins_compl_len())
				   : STRNCMP(p, pat, ins_compl_len())) == 0)
		return OK;
	}
    }
    return FAIL;
}