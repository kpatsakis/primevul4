yank_register_mline(int regname)
{
    if (regname != 0 && !valid_yank_reg(regname, FALSE))
	return FALSE;
    if (regname == '_')		// black hole is always empty
	return FALSE;
    get_yank_register(regname, FALSE);
    return (y_current->y_type == MLINE);
}