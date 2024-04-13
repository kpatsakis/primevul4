getreg_wrap_one_line(char_u *s, int flags)
{
    if (flags & GREG_LIST)
    {
	list_T *list = list_alloc();

	if (list != NULL)
	{
	    if (list_append_string(list, NULL, -1) == FAIL)
	    {
		list_free(list);
		return NULL;
	    }
	    list->lv_first->li_tv.vval.v_string = s;
	}
	return (char_u *)list;
    }
    return s;
}