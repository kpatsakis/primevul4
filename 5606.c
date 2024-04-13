init_write_reg(
    int		name,
    yankreg_T	**old_y_previous,
    yankreg_T	**old_y_current,
    int		must_append,
    int		*yank_type UNUSED)
{
    if (!valid_yank_reg(name, TRUE))	    // check for valid reg name
    {
	emsg_invreg(name);
	return FAIL;
    }

    // Don't want to change the current (unnamed) register
    *old_y_previous = y_previous;
    *old_y_current = y_current;

    get_yank_register(name, TRUE);
    if (!y_append && !must_append)
	free_yank_all();
    return OK;
}