override_value(option, priority, source)
    const char *option;
    int priority;
    const char *source;
{
	option_t *opt;

	opt = find_option(option);
	if (opt == NULL)
		return 0;
	while (opt->flags & OPT_PRIOSUB)
		--opt;
	if ((opt->flags & OPT_PRIO) && priority < opt->priority)
		return 0;
	opt->priority = priority;
	opt->source = source;
	opt->winner = -1;
	return 1;
}
