init_yank(void)
{
    int		i;

    for (i = 0; i < NUM_REGISTERS; ++i)
	y_regs[i].y_array = NULL;
}