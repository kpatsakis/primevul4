stuff_yank(int regname, char_u *p)
{
    char_u	*lp;
    char_u	**pp;

    // check for read-only register
    if (regname != 0 && !valid_yank_reg(regname, TRUE))
    {
	vim_free(p);
	return FAIL;
    }
    if (regname == '_')		    // black hole: don't do anything
    {
	vim_free(p);
	return OK;
    }
    get_yank_register(regname, TRUE);
    if (y_append && y_current->y_array != NULL)
    {
	pp = &(y_current->y_array[y_current->y_size - 1]);
	lp = alloc(STRLEN(*pp) + STRLEN(p) + 1);
	if (lp == NULL)
	{
	    vim_free(p);
	    return FAIL;
	}
	STRCPY(lp, *pp);
	STRCAT(lp, p);
	vim_free(p);
	vim_free(*pp);
	*pp = lp;
    }
    else
    {
	free_yank_all();
	if ((y_current->y_array = ALLOC_ONE(char_u *)) == NULL)
	{
	    vim_free(p);
	    return FAIL;
	}
	y_current->y_array[0] = p;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  // used to be MLINE, why?
#ifdef FEAT_VIMINFO
	y_current->y_time_set = vim_time();
#endif
    }
    return OK;
}