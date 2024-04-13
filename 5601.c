get_reg_type(int regname, long *reglen)
{
    switch (regname)
    {
	case '%':		// file name
	case '#':		// alternate file name
	case '=':		// expression
	case ':':		// last command line
	case '/':		// last search-pattern
	case '.':		// last inserted text
# ifdef FEAT_SEARCHPATH
	case Ctrl_F:		// Filename under cursor
	case Ctrl_P:		// Path under cursor, expand via "path"
# endif
	case Ctrl_W:		// word under cursor
	case Ctrl_A:		// WORD (mnemonic All) under cursor
	case '_':		// black hole: always empty
	    return MCHAR;
    }

# ifdef FEAT_CLIPBOARD
    regname = may_get_selection(regname);
# endif

    if (regname != NUL && !valid_yank_reg(regname, FALSE))
	return MAUTO;

    get_yank_register(regname, FALSE);

    if (y_current->y_array != NULL)
    {
	if (reglen != NULL && y_current->y_type == MBLOCK)
	    *reglen = y_current->y_width;
	return y_current->y_type;
    }
    return MAUTO;
}