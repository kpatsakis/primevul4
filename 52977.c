static int handle_revision_opt(struct rev_info *revs, int argc, const char **argv,
			       int *unkc, const char **unkv)
{
	const char *arg = argv[0];
	const char *optarg;
	int argcount;

	/* pseudo revision arguments */
	if (!strcmp(arg, "--all") || !strcmp(arg, "--branches") ||
	    !strcmp(arg, "--tags") || !strcmp(arg, "--remotes") ||
	    !strcmp(arg, "--reflog") || !strcmp(arg, "--not") ||
	    !strcmp(arg, "--no-walk") || !strcmp(arg, "--do-walk") ||
	    !strcmp(arg, "--bisect") || starts_with(arg, "--glob=") ||
	    !strcmp(arg, "--indexed-objects") ||
	    starts_with(arg, "--exclude=") ||
	    starts_with(arg, "--branches=") || starts_with(arg, "--tags=") ||
	    starts_with(arg, "--remotes=") || starts_with(arg, "--no-walk="))
	{
		unkv[(*unkc)++] = arg;
		return 1;
	}

	if ((argcount = parse_long_opt("max-count", argv, &optarg))) {
		revs->max_count = atoi(optarg);
		revs->no_walk = 0;
		return argcount;
	} else if ((argcount = parse_long_opt("skip", argv, &optarg))) {
		revs->skip_count = atoi(optarg);
		return argcount;
	} else if ((*arg == '-') && isdigit(arg[1])) {
		/* accept -<digit>, like traditional "head" */
		if (strtol_i(arg + 1, 10, &revs->max_count) < 0 ||
		    revs->max_count < 0)
			die("'%s': not a non-negative integer", arg + 1);
		revs->no_walk = 0;
	} else if (!strcmp(arg, "-n")) {
		if (argc <= 1)
			return error("-n requires an argument");
		revs->max_count = atoi(argv[1]);
		revs->no_walk = 0;
		return 2;
	} else if (starts_with(arg, "-n")) {
		revs->max_count = atoi(arg + 2);
		revs->no_walk = 0;
	} else if ((argcount = parse_long_opt("max-age", argv, &optarg))) {
		revs->max_age = atoi(optarg);
		return argcount;
	} else if ((argcount = parse_long_opt("since", argv, &optarg))) {
		revs->max_age = approxidate(optarg);
		return argcount;
	} else if ((argcount = parse_long_opt("after", argv, &optarg))) {
		revs->max_age = approxidate(optarg);
		return argcount;
	} else if ((argcount = parse_long_opt("min-age", argv, &optarg))) {
		revs->min_age = atoi(optarg);
		return argcount;
	} else if ((argcount = parse_long_opt("before", argv, &optarg))) {
		revs->min_age = approxidate(optarg);
		return argcount;
	} else if ((argcount = parse_long_opt("until", argv, &optarg))) {
		revs->min_age = approxidate(optarg);
		return argcount;
	} else if (!strcmp(arg, "--first-parent")) {
		revs->first_parent_only = 1;
	} else if (!strcmp(arg, "--ancestry-path")) {
		revs->ancestry_path = 1;
		revs->simplify_history = 0;
		revs->limited = 1;
	} else if (!strcmp(arg, "-g") || !strcmp(arg, "--walk-reflogs")) {
		init_reflog_walk(&revs->reflog_info);
	} else if (!strcmp(arg, "--default")) {
		if (argc <= 1)
			return error("bad --default argument");
		revs->def = argv[1];
		return 2;
	} else if (!strcmp(arg, "--merge")) {
		revs->show_merge = 1;
	} else if (!strcmp(arg, "--topo-order")) {
		revs->sort_order = REV_SORT_IN_GRAPH_ORDER;
		revs->topo_order = 1;
	} else if (!strcmp(arg, "--simplify-merges")) {
		revs->simplify_merges = 1;
		revs->topo_order = 1;
		revs->rewrite_parents = 1;
		revs->simplify_history = 0;
		revs->limited = 1;
	} else if (!strcmp(arg, "--simplify-by-decoration")) {
		revs->simplify_merges = 1;
		revs->topo_order = 1;
		revs->rewrite_parents = 1;
		revs->simplify_history = 0;
		revs->simplify_by_decoration = 1;
		revs->limited = 1;
		revs->prune = 1;
		load_ref_decorations(DECORATE_SHORT_REFS);
	} else if (!strcmp(arg, "--date-order")) {
		revs->sort_order = REV_SORT_BY_COMMIT_DATE;
		revs->topo_order = 1;
	} else if (!strcmp(arg, "--author-date-order")) {
		revs->sort_order = REV_SORT_BY_AUTHOR_DATE;
		revs->topo_order = 1;
	} else if (starts_with(arg, "--early-output")) {
		int count = 100;
		switch (arg[14]) {
		case '=':
			count = atoi(arg+15);
			/* Fallthrough */
		case 0:
			revs->topo_order = 1;
		       revs->early_output = count;
		}
	} else if (!strcmp(arg, "--parents")) {
		revs->rewrite_parents = 1;
		revs->print_parents = 1;
	} else if (!strcmp(arg, "--dense")) {
		revs->dense = 1;
	} else if (!strcmp(arg, "--sparse")) {
		revs->dense = 0;
	} else if (!strcmp(arg, "--show-all")) {
		revs->show_all = 1;
	} else if (!strcmp(arg, "--remove-empty")) {
		revs->remove_empty_trees = 1;
	} else if (!strcmp(arg, "--merges")) {
		revs->min_parents = 2;
	} else if (!strcmp(arg, "--no-merges")) {
		revs->max_parents = 1;
	} else if (starts_with(arg, "--min-parents=")) {
		revs->min_parents = atoi(arg+14);
	} else if (starts_with(arg, "--no-min-parents")) {
		revs->min_parents = 0;
	} else if (starts_with(arg, "--max-parents=")) {
		revs->max_parents = atoi(arg+14);
	} else if (starts_with(arg, "--no-max-parents")) {
		revs->max_parents = -1;
	} else if (!strcmp(arg, "--boundary")) {
		revs->boundary = 1;
	} else if (!strcmp(arg, "--left-right")) {
		revs->left_right = 1;
	} else if (!strcmp(arg, "--left-only")) {
		if (revs->right_only)
			die("--left-only is incompatible with --right-only"
			    " or --cherry");
		revs->left_only = 1;
	} else if (!strcmp(arg, "--right-only")) {
		if (revs->left_only)
			die("--right-only is incompatible with --left-only");
		revs->right_only = 1;
	} else if (!strcmp(arg, "--cherry")) {
		if (revs->left_only)
			die("--cherry is incompatible with --left-only");
		revs->cherry_mark = 1;
		revs->right_only = 1;
		revs->max_parents = 1;
		revs->limited = 1;
	} else if (!strcmp(arg, "--count")) {
		revs->count = 1;
	} else if (!strcmp(arg, "--cherry-mark")) {
		if (revs->cherry_pick)
			die("--cherry-mark is incompatible with --cherry-pick");
		revs->cherry_mark = 1;
		revs->limited = 1; /* needs limit_list() */
	} else if (!strcmp(arg, "--cherry-pick")) {
		if (revs->cherry_mark)
			die("--cherry-pick is incompatible with --cherry-mark");
		revs->cherry_pick = 1;
		revs->limited = 1;
	} else if (!strcmp(arg, "--objects")) {
		revs->tag_objects = 1;
		revs->tree_objects = 1;
		revs->blob_objects = 1;
	} else if (!strcmp(arg, "--objects-edge")) {
		revs->tag_objects = 1;
		revs->tree_objects = 1;
		revs->blob_objects = 1;
		revs->edge_hint = 1;
	} else if (!strcmp(arg, "--objects-edge-aggressive")) {
		revs->tag_objects = 1;
		revs->tree_objects = 1;
		revs->blob_objects = 1;
		revs->edge_hint = 1;
		revs->edge_hint_aggressive = 1;
	} else if (!strcmp(arg, "--verify-objects")) {
		revs->tag_objects = 1;
		revs->tree_objects = 1;
		revs->blob_objects = 1;
		revs->verify_objects = 1;
	} else if (!strcmp(arg, "--unpacked")) {
		revs->unpacked = 1;
	} else if (starts_with(arg, "--unpacked=")) {
		die("--unpacked=<packfile> no longer supported.");
	} else if (!strcmp(arg, "-r")) {
		revs->diff = 1;
		DIFF_OPT_SET(&revs->diffopt, RECURSIVE);
	} else if (!strcmp(arg, "-t")) {
		revs->diff = 1;
		DIFF_OPT_SET(&revs->diffopt, RECURSIVE);
		DIFF_OPT_SET(&revs->diffopt, TREE_IN_RECURSIVE);
	} else if (!strcmp(arg, "-m")) {
		revs->ignore_merges = 0;
	} else if (!strcmp(arg, "-c")) {
		revs->diff = 1;
		revs->dense_combined_merges = 0;
		revs->combine_merges = 1;
	} else if (!strcmp(arg, "--cc")) {
		revs->diff = 1;
		revs->dense_combined_merges = 1;
		revs->combine_merges = 1;
	} else if (!strcmp(arg, "-v")) {
		revs->verbose_header = 1;
	} else if (!strcmp(arg, "--pretty")) {
		revs->verbose_header = 1;
		revs->pretty_given = 1;
		get_commit_format(NULL, revs);
	} else if (starts_with(arg, "--pretty=") || starts_with(arg, "--format=")) {
		/*
		 * Detached form ("--pretty X" as opposed to "--pretty=X")
		 * not allowed, since the argument is optional.
		 */
		revs->verbose_header = 1;
		revs->pretty_given = 1;
		get_commit_format(arg+9, revs);
	} else if (!strcmp(arg, "--show-notes") || !strcmp(arg, "--notes")) {
		revs->show_notes = 1;
		revs->show_notes_given = 1;
		revs->notes_opt.use_default_notes = 1;
	} else if (!strcmp(arg, "--show-signature")) {
		revs->show_signature = 1;
	} else if (!strcmp(arg, "--show-linear-break") ||
		   starts_with(arg, "--show-linear-break=")) {
		if (starts_with(arg, "--show-linear-break="))
			revs->break_bar = xstrdup(arg + 20);
		else
			revs->break_bar = "                    ..........";
		revs->track_linear = 1;
		revs->track_first_time = 1;
	} else if (starts_with(arg, "--show-notes=") ||
		   starts_with(arg, "--notes=")) {
		struct strbuf buf = STRBUF_INIT;
		revs->show_notes = 1;
		revs->show_notes_given = 1;
		if (starts_with(arg, "--show-notes")) {
			if (revs->notes_opt.use_default_notes < 0)
				revs->notes_opt.use_default_notes = 1;
			strbuf_addstr(&buf, arg+13);
		}
		else
			strbuf_addstr(&buf, arg+8);
		expand_notes_ref(&buf);
		string_list_append(&revs->notes_opt.extra_notes_refs,
				   strbuf_detach(&buf, NULL));
	} else if (!strcmp(arg, "--no-notes")) {
		revs->show_notes = 0;
		revs->show_notes_given = 1;
		revs->notes_opt.use_default_notes = -1;
		/* we have been strdup'ing ourselves, so trick
		 * string_list into free()ing strings */
		revs->notes_opt.extra_notes_refs.strdup_strings = 1;
		string_list_clear(&revs->notes_opt.extra_notes_refs, 0);
		revs->notes_opt.extra_notes_refs.strdup_strings = 0;
	} else if (!strcmp(arg, "--standard-notes")) {
		revs->show_notes_given = 1;
		revs->notes_opt.use_default_notes = 1;
	} else if (!strcmp(arg, "--no-standard-notes")) {
		revs->notes_opt.use_default_notes = 0;
	} else if (!strcmp(arg, "--oneline")) {
		revs->verbose_header = 1;
		get_commit_format("oneline", revs);
		revs->pretty_given = 1;
		revs->abbrev_commit = 1;
	} else if (!strcmp(arg, "--graph")) {
		revs->topo_order = 1;
		revs->rewrite_parents = 1;
		revs->graph = graph_init(revs);
	} else if (!strcmp(arg, "--root")) {
		revs->show_root_diff = 1;
	} else if (!strcmp(arg, "--no-commit-id")) {
		revs->no_commit_id = 1;
	} else if (!strcmp(arg, "--always")) {
		revs->always_show_header = 1;
	} else if (!strcmp(arg, "--no-abbrev")) {
		revs->abbrev = 0;
	} else if (!strcmp(arg, "--abbrev")) {
		revs->abbrev = DEFAULT_ABBREV;
	} else if (starts_with(arg, "--abbrev=")) {
		revs->abbrev = strtoul(arg + 9, NULL, 10);
		if (revs->abbrev < MINIMUM_ABBREV)
			revs->abbrev = MINIMUM_ABBREV;
		else if (revs->abbrev > 40)
			revs->abbrev = 40;
	} else if (!strcmp(arg, "--abbrev-commit")) {
		revs->abbrev_commit = 1;
		revs->abbrev_commit_given = 1;
	} else if (!strcmp(arg, "--no-abbrev-commit")) {
		revs->abbrev_commit = 0;
	} else if (!strcmp(arg, "--full-diff")) {
		revs->diff = 1;
		revs->full_diff = 1;
	} else if (!strcmp(arg, "--full-history")) {
		revs->simplify_history = 0;
	} else if (!strcmp(arg, "--relative-date")) {
		revs->date_mode.type = DATE_RELATIVE;
		revs->date_mode_explicit = 1;
	} else if ((argcount = parse_long_opt("date", argv, &optarg))) {
		parse_date_format(optarg, &revs->date_mode);
		revs->date_mode_explicit = 1;
		return argcount;
	} else if (!strcmp(arg, "--log-size")) {
		revs->show_log_size = 1;
	}
	/*
	 * Grepping the commit log
	 */
	else if ((argcount = parse_long_opt("author", argv, &optarg))) {
		add_header_grep(revs, GREP_HEADER_AUTHOR, optarg);
		return argcount;
	} else if ((argcount = parse_long_opt("committer", argv, &optarg))) {
		add_header_grep(revs, GREP_HEADER_COMMITTER, optarg);
		return argcount;
	} else if ((argcount = parse_long_opt("grep-reflog", argv, &optarg))) {
		add_header_grep(revs, GREP_HEADER_REFLOG, optarg);
		return argcount;
	} else if ((argcount = parse_long_opt("grep", argv, &optarg))) {
		add_message_grep(revs, optarg);
		return argcount;
	} else if (!strcmp(arg, "--grep-debug")) {
		revs->grep_filter.debug = 1;
	} else if (!strcmp(arg, "--basic-regexp")) {
		grep_set_pattern_type_option(GREP_PATTERN_TYPE_BRE, &revs->grep_filter);
	} else if (!strcmp(arg, "--extended-regexp") || !strcmp(arg, "-E")) {
		grep_set_pattern_type_option(GREP_PATTERN_TYPE_ERE, &revs->grep_filter);
	} else if (!strcmp(arg, "--regexp-ignore-case") || !strcmp(arg, "-i")) {
		revs->grep_filter.regflags |= REG_ICASE;
		DIFF_OPT_SET(&revs->diffopt, PICKAXE_IGNORE_CASE);
	} else if (!strcmp(arg, "--fixed-strings") || !strcmp(arg, "-F")) {
		grep_set_pattern_type_option(GREP_PATTERN_TYPE_FIXED, &revs->grep_filter);
	} else if (!strcmp(arg, "--perl-regexp")) {
		grep_set_pattern_type_option(GREP_PATTERN_TYPE_PCRE, &revs->grep_filter);
	} else if (!strcmp(arg, "--all-match")) {
		revs->grep_filter.all_match = 1;
	} else if (!strcmp(arg, "--invert-grep")) {
		revs->invert_grep = 1;
	} else if ((argcount = parse_long_opt("encoding", argv, &optarg))) {
		if (strcmp(optarg, "none"))
			git_log_output_encoding = xstrdup(optarg);
		else
			git_log_output_encoding = "";
		return argcount;
	} else if (!strcmp(arg, "--reverse")) {
		revs->reverse ^= 1;
	} else if (!strcmp(arg, "--children")) {
		revs->children.name = "children";
		revs->limited = 1;
	} else if (!strcmp(arg, "--ignore-missing")) {
		revs->ignore_missing = 1;
	} else {
		int opts = diff_opt_parse(&revs->diffopt, argv, argc);
		if (!opts)
			unkv[(*unkc)++] = arg;
		return opts;
	}
	if (revs->graph && revs->track_linear)
		die("--show-linear-break and --graph are incompatible");

	return 1;
}
