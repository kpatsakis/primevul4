check_prevcol(
    char_u	*linep,
    int		col,
    int		ch,
    int		*prevcol)
{
    --col;
    if (col > 0 && has_mbyte)
	col -= (*mb_head_off)(linep, linep + col);
    if (prevcol)
	*prevcol = col;
    return (col >= 0 && linep[col] == ch) ? TRUE : FALSE;
}