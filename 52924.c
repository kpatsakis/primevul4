static void parse_argv(void)
{
	unsigned int i;

	for (i = 1; i < global_argc; i++) {
		const char *a = global_argv[i];

		if (*a != '-' || !strcmp(a, "--"))
			break;

		if (!skip_prefix(a, "--", &a))
			die("unknown option %s", a);

		if (parse_one_option(a))
			continue;

		if (parse_one_feature(a, 0))
			continue;

		if (skip_prefix(a, "cat-blob-fd=", &a)) {
			option_cat_blob_fd(a);
			continue;
		}

		die("unknown option --%s", a);
	}
	if (i != global_argc)
		usage(fast_import_usage);

	seen_data_command = 1;
	if (import_marks_file)
		read_marks();
}
