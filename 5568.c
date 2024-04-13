ignorecase_opt(char_u *pat, int ic_in, int scs)
{
    int		ic = ic_in;

    if (ic && !no_smartcase && scs
			    && !(ctrl_x_mode_not_default() && curbuf->b_p_inf))
	ic = !pat_has_uppercase(pat);
    no_smartcase = FALSE;

    return ic;
}