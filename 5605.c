get_register(
    int		name,
    int		copy)	// make a copy, if FALSE make register empty.
{
    yankreg_T	*reg;
    int		i;

#ifdef FEAT_CLIPBOARD
    // When Visual area changed, may have to update selection.  Obtain the
    // selection too.
    if (name == '*' && clip_star.available)
    {
	if (clip_isautosel_star())
	    clip_update_selection(&clip_star);
	may_get_selection(name);
    }
    if (name == '+' && clip_plus.available)
    {
	if (clip_isautosel_plus())
	    clip_update_selection(&clip_plus);
	may_get_selection(name);
    }
#endif

    get_yank_register(name, 0);
    reg = ALLOC_ONE(yankreg_T);
    if (reg != NULL)
    {
	*reg = *y_current;
	if (copy)
	{
	    // If we run out of memory some or all of the lines are empty.
	    if (reg->y_size == 0)
		reg->y_array = NULL;
	    else
		reg->y_array = ALLOC_MULT(char_u *, reg->y_size);
	    if (reg->y_array != NULL)
	    {
		for (i = 0; i < reg->y_size; ++i)
		    reg->y_array[i] = vim_strsave(y_current->y_array[i]);
	    }
	}
	else
	    y_current->y_array = NULL;
    }
    return (void *)reg;
}