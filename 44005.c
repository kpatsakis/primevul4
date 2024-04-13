n_arguments(opt)
    option_t *opt;
{
	return (opt->type == o_bool || opt->type == o_special_noarg
		|| (opt->flags & OPT_NOARG))? 0: 1;
}
