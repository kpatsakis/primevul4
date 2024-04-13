copy_yank_reg(yankreg_T *reg)
{
    yankreg_T	*curr = y_current;
    long	j;

    y_current = reg;
    free_yank_all();
    *y_current = *curr;
    y_current->y_array = lalloc_clear(
				    sizeof(char_u *) * y_current->y_size, TRUE);
    if (y_current->y_array == NULL)
	y_current->y_size = 0;
    else
	for (j = 0; j < y_current->y_size; ++j)
	    if ((y_current->y_array[j] = vim_strsave(curr->y_array[j])) == NULL)
	    {
		free_yank(j);
		y_current->y_size = 0;
		break;
	    }
    y_current = curr;
}