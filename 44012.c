parse_args(argc, argv)
    int argc;
    char **argv;
{
    char *arg;
    option_t *opt;
    int n;

    privileged_option = privileged;
    option_source = "command line";
    option_priority = OPRIO_CMDLINE;
    while (argc > 0) {
	arg = *argv++;
	--argc;
	opt = find_option(arg);
	if (opt == NULL) {
	    option_error("unrecognized option '%s'", arg);
	    usage();
	    return 0;
	}
	n = n_arguments(opt);
	if (argc < n) {
	    option_error("too few parameters for option %s", arg);
	    return 0;
	}
	if (!process_option(opt, arg, argv))
	    return 0;
	argc -= n;
	argv += n;
    }
    return 1;
}
