match_option(name, opt, dowild)
    char *name;
    option_t *opt;
    int dowild;
{
	int (*match) __P((char *, char **, int));

	if (dowild != (opt->type == o_wild))
		return 0;
	if (!dowild)
		return strcmp(name, opt->name) == 0;
	match = (int (*) __P((char *, char **, int))) opt->addr;
	return (*match)(name, NULL, 0);
}
