do_record(int c)
{
    char_u	    *p;
    static int	    regname;
    yankreg_T	    *old_y_previous, *old_y_current;
    int		    retval;

    if (reg_recording == 0)	    // start recording
    {
	// registers 0-9, a-z and " are allowed
	if (c < 0 || (!ASCII_ISALNUM(c) && c != '"'))
	    retval = FAIL;
	else
	{
	    reg_recording = c;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    else			    // stop recording
    {
	// Get the recorded key hits.  K_SPECIAL and CSI will be escaped, this
	// needs to be removed again to put it in a register.  exec_reg then
	// adds the escaping back later.
	reg_recording = 0;
	msg("");
	p = get_recorded();
	if (p == NULL)
	    retval = FAIL;
	else
	{
	    // Remove escaping for CSI and K_SPECIAL in multi-byte chars.
	    vim_unescape_csi(p);

	    // We don't want to change the default register here, so save and
	    // restore the current register name.
	    old_y_previous = y_previous;
	    old_y_current = y_current;

	    retval = stuff_yank(regname, p);

	    y_previous = old_y_previous;
	    y_current = old_y_current;
	}
    }
    return retval;
}