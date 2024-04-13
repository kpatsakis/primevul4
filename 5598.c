reverse_text(char_u *s)
{
    unsigned	len;
    unsigned	s_i, rev_i;
    char_u	*rev;

    /*
     * Reverse the pattern.
     */
    len = (unsigned)STRLEN(s);
    rev = alloc(len + 1);
    if (rev != NULL)
    {
	rev_i = len;
	for (s_i = 0; s_i < len; ++s_i)
	{
	    if (has_mbyte)
	    {
		int	mb_len;

		mb_len = (*mb_ptr2len)(s + s_i);
		rev_i -= mb_len;
		mch_memmove(rev + rev_i, s + s_i, mb_len);
		s_i += mb_len - 1;
	    }
	    else
		rev[--rev_i] = s[s_i];

	}
	rev[len] = NUL;
    }
    return rev;
}