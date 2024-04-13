print_options(printer, arg)
    printer_func printer;
    void *arg;
{
	struct option_list *list;
	int i;

	printer(arg, "pppd options in effect:\n");
	print_option_list(general_options, printer, arg);
	print_option_list(auth_options, printer, arg);
	for (list = extra_options; list != NULL; list = list->next)
		print_option_list(list->options, printer, arg);
	print_option_list(the_channel->options, printer, arg);
	for (i = 0; protocols[i] != NULL; ++i)
		print_option_list(protocols[i]->options, printer, arg);
}
