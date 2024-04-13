yank_do_autocmd(oparg_T *oap, yankreg_T *reg)
{
    static int	    recursive = FALSE;
    dict_T	    *v_event;
    list_T	    *list;
    int		    n;
    char_u	    buf[NUMBUFLEN + 2];
    long	    reglen = 0;
    save_v_event_T  save_v_event;

    if (recursive)
	return;

    v_event = get_v_event(&save_v_event);

    list = list_alloc();
    if (list == NULL)
	return;
    for (n = 0; n < reg->y_size; n++)
	list_append_string(list, reg->y_array[n], -1);
    list->lv_lock = VAR_FIXED;
    (void)dict_add_list(v_event, "regcontents", list);

    buf[0] = (char_u)oap->regname;
    buf[1] = NUL;
    (void)dict_add_string(v_event, "regname", buf);

    buf[0] = get_op_char(oap->op_type);
    buf[1] = get_extra_op_char(oap->op_type);
    buf[2] = NUL;
    (void)dict_add_string(v_event, "operator", buf);

    buf[0] = NUL;
    buf[1] = NUL;
    switch (get_reg_type(oap->regname, &reglen))
    {
	case MLINE: buf[0] = 'V'; break;
	case MCHAR: buf[0] = 'v'; break;
	case MBLOCK:
		vim_snprintf((char *)buf, sizeof(buf), "%c%ld", Ctrl_V,
			     reglen + 1);
		break;
    }
    (void)dict_add_string(v_event, "regtype", buf);

    (void)dict_add_bool(v_event, "visual", oap->is_VIsual);

    // Lock the dictionary and its keys
    dict_set_items_ro(v_event);

    recursive = TRUE;
    textwinlock++;
    apply_autocmds(EVENT_TEXTYANKPOST, NULL, NULL, FALSE, curbuf);
    textwinlock--;
    recursive = FALSE;

    // Empty the dictionary, v:event is still valid
    restore_v_event(v_event, &save_v_event);
}