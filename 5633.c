get_yank_register(int regname, int writing)
{
    int	    i;
    int	    ret = FALSE;

    y_append = FALSE;
    if ((regname == 0 || regname == '"') && !writing && y_previous != NULL)
    {
	y_current = y_previous;
	return ret;
    }
    i = regname;
    if (VIM_ISDIGIT(i))
	i -= '0';
    else if (ASCII_ISLOWER(i))
	i = CharOrdLow(i) + 10;
    else if (ASCII_ISUPPER(i))
    {
	i = CharOrdUp(i) + 10;
	y_append = TRUE;
    }
    else if (regname == '-')
	i = DELETION_REGISTER;
#ifdef FEAT_CLIPBOARD
    // When selection is not available, use register 0 instead of '*'
    else if (clip_star.available && regname == '*')
    {
	i = STAR_REGISTER;
	ret = TRUE;
    }
    // When clipboard is not available, use register 0 instead of '+'
    else if (clip_plus.available && regname == '+')
    {
	i = PLUS_REGISTER;
	ret = TRUE;
    }
#endif
#ifdef FEAT_DND
    else if (!writing && regname == '~')
	i = TILDE_REGISTER;
#endif
    else		// not 0-9, a-z, A-Z or '-': use register 0
	i = 0;
    y_current = &(y_regs[i]);
    if (writing)	// remember the register we write into for do_put()
	y_previous = y_current;
    return ret;
}