free_yank(long n)
{
    if (y_current->y_array != NULL)
    {
	long	    i;

	for (i = n; --i >= 0; )
	    vim_free(y_current->y_array[i]);
	VIM_CLEAR(y_current->y_array);
    }
}