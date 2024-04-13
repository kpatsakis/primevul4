valid_yank_reg(
    int	    regname,
    int	    writing)	    // if TRUE check for writable registers
{
    if (       (regname > 0 && ASCII_ISALNUM(regname))
	    || (!writing && vim_strchr((char_u *)
#ifdef FEAT_EVAL
				    "/.%:="
#else
				    "/.%:"
#endif
					, regname) != NULL)
	    || regname == '#'
	    || regname == '"'
	    || regname == '-'
	    || regname == '_'
#ifdef FEAT_CLIPBOARD
	    || regname == '*'
	    || regname == '+'
#endif
#ifdef FEAT_DND
	    || (!writing && regname == '~')
#endif
							)
	return TRUE;
    return FALSE;
}