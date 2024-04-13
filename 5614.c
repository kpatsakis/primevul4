shift_delete_registers()
{
    int		n;

    y_current = &y_regs[9];
    free_yank_all();			// free register nine
    for (n = 9; n > 1; --n)
	y_regs[n] = y_regs[n - 1];
    y_current = &y_regs[1];
    if (!y_append)
	y_previous = y_current;
    y_regs[1].y_array = NULL;		// set register one to empty
}