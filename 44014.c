print_option_list(opt, printer, arg)
    option_t *opt;
    printer_func printer;
    void *arg;
{
	while (opt->name != NULL) {
		if (opt->priority != OPRIO_DEFAULT
		    && opt->winner != (short int) -1)
			print_option(opt + opt->winner, opt, printer, arg);
		do {
			++opt;
		} while (opt->flags & OPT_PRIOSUB);
	}
}
