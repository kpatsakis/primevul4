find_option(name)
    const char *name;
{
	option_t *opt;
	struct option_list *list;
	int i, dowild;

	for (dowild = 0; dowild <= 1; ++dowild) {
		for (opt = general_options; opt->name != NULL; ++opt)
			if (match_option(name, opt, dowild))
				return opt;
		for (opt = auth_options; opt->name != NULL; ++opt)
			if (match_option(name, opt, dowild))
				return opt;
		for (list = extra_options; list != NULL; list = list->next)
			for (opt = list->options; opt->name != NULL; ++opt)
				if (match_option(name, opt, dowild))
					return opt;
		for (opt = the_channel->options; opt->name != NULL; ++opt)
			if (match_option(name, opt, dowild))
				return opt;
		for (i = 0; protocols[i] != NULL; ++i)
			if ((opt = protocols[i]->options) != NULL)
				for (; opt->name != NULL; ++opt)
					if (match_option(name, opt, dowild))
						return opt;
	}
	return NULL;
}
