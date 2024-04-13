user_setprint(opt, printer, arg)
    option_t *opt;
    printer_func printer;
    void *arg;
{
    struct userenv *uep, *uepnext;

    uepnext = userenv_list;
    while (uepnext != NULL && !uepnext->ue_isset)
	uepnext = uepnext->ue_next;
    while ((uep = uepnext) != NULL) {
	uepnext = uep->ue_next;
	while (uepnext != NULL && !uepnext->ue_isset)
	    uepnext = uepnext->ue_next;
	(*printer)(arg, "%s=%s", uep->ue_name, uep->ue_value);
	if (uepnext != NULL)
	    (*printer)(arg, "\t\t# (from %s)\n%s ", uep->ue_source, opt->name);
	else
	    opt->source = uep->ue_source;
    }
}
