cmdline_paste_reg(
    int regname,
    int literally_arg,	// Insert text literally instead of "as typed"
    int remcr)		// don't add CR characters
{
    long	i;
    int		literally = literally_arg;

    if (get_yank_register(regname, FALSE))
	literally = TRUE;
    if (y_current->y_array == NULL)
	return FAIL;

    for (i = 0; i < y_current->y_size; ++i)
    {
	cmdline_paste_str(y_current->y_array[i], literally);

	// Insert ^M between lines and after last line if type is MLINE.
	// Don't do this when "remcr" is TRUE.
	if ((y_current->y_type == MLINE || i < y_current->y_size - 1) && !remcr)
	    cmdline_paste_str((char_u *)"\r", literally);

	// Check for CTRL-C, in case someone tries to paste a few thousand
	// lines and gets bored.
	ui_breakcheck();
	if (got_int)
	    return FAIL;
    }
    return OK;
}