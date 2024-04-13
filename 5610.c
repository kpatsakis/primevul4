clear_registers(void)
{
    int		i;

    for (i = 0; i < NUM_REGISTERS; ++i)
    {
	y_current = &y_regs[i];
	if (y_current->y_array != NULL)
	    free_yank_all();
    }
}