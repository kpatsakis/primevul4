dis_msg(
    char_u	*p,
    int		skip_esc)	    // if TRUE, ignore trailing ESC
{
    int		n;
    int		l;

    n = (int)Columns - 6;
    while (*p != NUL
	    && !(*p == ESC && skip_esc && *(p + 1) == NUL)
	    && (n -= ptr2cells(p)) >= 0)
    {
	if (has_mbyte && (l = (*mb_ptr2len)(p)) > 1)
	{
	    msg_outtrans_len(p, l);
	    p += l;
	}
	else
	    msg_outtrans_len(p++, 1);
    }
    ui_breakcheck();
}