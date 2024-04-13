put_reedit_in_typebuf(int silent)
{
    char_u	buf[3];

    if (restart_edit != NUL)
    {
	if (restart_edit == 'V')
	{
	    buf[0] = 'g';
	    buf[1] = 'R';
	    buf[2] = NUL;
	}
	else
	{
	    buf[0] = restart_edit == 'I' ? 'i' : restart_edit;
	    buf[1] = NUL;
	}
	if (ins_typebuf(buf, REMAP_NONE, 0, TRUE, silent) == OK)
	    restart_edit = NUL;
    }
}