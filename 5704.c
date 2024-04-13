static int read_arguments(int argc, char **argv)
{
	int optchar;
	char *arg1 = argv[1];
	char *op = NULL;
	char *cp;
	const char *opt_string = OPTION_STRING;
	char site_arg[INET_ADDRSTRLEN] = {0};
	int left;

	cl.type = 0;
	if ((cp = strstr(argv[0], ATTR_PROG)) &&
			!strcmp(cp, ATTR_PROG)) {
		cl.type = GEOSTORE;
		op = argv[1];
		optind = 2;
		opt_string = ATTR_OPTION_STRING;
	} else if (argc > 1 && (strcmp(arg1, "arbitrator") == 0 ||
			strcmp(arg1, "site") == 0 ||
			strcmp(arg1, "start") == 0 ||
			strcmp(arg1, "daemon") == 0)) {
		cl.type = DAEMON;
		optind = 2;
	} else if (argc > 1 && (strcmp(arg1, "status") == 0)) {
		cl.type = STATUS;
		optind = 2;
	} else if (argc > 1 && (strcmp(arg1, "client") == 0)) {
		cl.type = CLIENT;
		if (argc < 3) {
			print_usage();
			exit(EXIT_FAILURE);
		}
		op = argv[2];
		optind = 3;
	}
	if (!cl.type) {
		cl.type = CLIENT;
		op = argv[1];
		optind = 2;
    }

	if (argc < 2 || !strcmp(arg1, "help") || !strcmp(arg1, "--help") ||
			!strcmp(arg1, "-h")) {
		if (cl.type == GEOSTORE)
			print_geostore_usage();
		else
			print_usage();
		exit(EXIT_SUCCESS);
	}

	if (!strcmp(arg1, "version") || !strcmp(arg1, "--version") ||
			!strcmp(arg1, "-V")) {
		printf("%s %s\n", argv[0], RELEASE_STR);
		exit(EXIT_SUCCESS);
	}

    if (cl.type == CLIENT) {
		if (!strcmp(op, "list"))
			cl.op = CMD_LIST;
		else if (!strcmp(op, "grant"))
			cl.op = CMD_GRANT;
		else if (!strcmp(op, "revoke"))
			cl.op = CMD_REVOKE;
		else if (!strcmp(op, "peers"))
			cl.op = CMD_PEERS;
		else {
			fprintf(stderr, "client operation \"%s\" is unknown\n",
					op);
			exit(EXIT_FAILURE);
		}
	} else if (cl.type == GEOSTORE) {
		if (!strcmp(op, "list"))
			cl.op = ATTR_LIST;
		else if (!strcmp(op, "set"))
			cl.op = ATTR_SET;
		else if (!strcmp(op, "get"))
			cl.op = ATTR_GET;
		else if (!strcmp(op, "delete"))
			cl.op = ATTR_DEL;
		else {
			fprintf(stderr, "attribute operation \"%s\" is unknown\n",
					op);
			exit(EXIT_FAILURE);
		}
	}

	while (optind < argc) {
		optchar = getopt(argc, argv, opt_string);

		switch (optchar) {
		case 'c':
			if (strchr(optarg, '/')) {
				safe_copy(cl.configfile, optarg,
						sizeof(cl.configfile), "config file");
			} else {
				/* If no "/" in there, use with default directory. */
				strcpy(cl.configfile, BOOTH_DEFAULT_CONF_DIR);
				cp = cl.configfile + strlen(BOOTH_DEFAULT_CONF_DIR);
				assert(cp > cl.configfile);
				assert(*(cp-1) == '/');

				/* Write at the \0, ie. after the "/" */
				safe_copy(cp, optarg,
						(sizeof(cl.configfile) -
						 (cp -  cl.configfile) -
						 strlen(BOOTH_DEFAULT_CONF_EXT)),
						"config name");

				/* If no extension, append ".conf".
				 * Space is available, see -strlen() above. */
				if (!strchr(cp, '.'))
					strcat(cp, BOOTH_DEFAULT_CONF_EXT);
			}
			break;

		case 'D':
			debug_level++;
			break;

		case 'S':
			daemonize = 0;
			enable_stderr = 1;
			break;

		case 'l':
			safe_copy(cl.lockfile, optarg, sizeof(cl.lockfile), "lock file");
			break;
		case 't':
			if (cl.op == CMD_GRANT || cl.op == CMD_REVOKE) {
				safe_copy(cl.msg.ticket.id, optarg,
						sizeof(cl.msg.ticket.id), "ticket name");
			} else if (cl.type == GEOSTORE) {
				safe_copy(cl.attr_msg.attr.tkt_id, optarg,
						sizeof(cl.attr_msg.attr.tkt_id), "ticket name");
			} else {
				print_usage();
				exit(EXIT_FAILURE);
			}
			break;

		case 's':
			/* For testing and debugging: allow "-s site" also for
			 * daemon start, so that the address that should be used
			 * can be set manually.
			 * This makes it easier to start multiple processes
			 * on one machine. */
			if (cl.type == CLIENT || cl.type == GEOSTORE ||
					(cl.type == DAEMON && debug_level)) {
				if (strcmp(optarg, OTHER_SITE) &&
						host_convert(optarg, site_arg, INET_ADDRSTRLEN) == 0) {
					safe_copy(cl.site, site_arg, sizeof(cl.site), "site name");
				} else {
					safe_copy(cl.site, optarg, sizeof(cl.site), "site name");
				}
			} else {
				log_error("\"-s\" not allowed in daemon mode.");
				exit(EXIT_FAILURE);
			}
			break;

		case 'F':
			if (cl.type != CLIENT || cl.op != CMD_GRANT) {
				log_error("use \"-F\" only for client grant");
				exit(EXIT_FAILURE);
			}
			cl.options |= OPT_IMMEDIATE;
			break;

		case 'w':
			if (cl.type != CLIENT ||
					(cl.op != CMD_GRANT && cl.op != CMD_REVOKE)) {
				log_error("use \"-w\" only for grant and revoke");
				exit(EXIT_FAILURE);
			}
			cl.options |= OPT_WAIT;
			break;

		case 'C':
			if (cl.type != CLIENT || cl.op != CMD_GRANT) {
				log_error("use \"-C\" only for grant");
				exit(EXIT_FAILURE);
			}
			cl.options |= OPT_WAIT | OPT_WAIT_COMMIT;
			break;

		case 'h':
			if (cl.type == GEOSTORE)
				print_geostore_usage();
			else
				print_usage();
			exit(EXIT_SUCCESS);
			break;

		case ':':
		case '?':
			fprintf(stderr, "Please use '-h' for usage.\n");
			exit(EXIT_FAILURE);
			break;

		case -1:
			/* No more parameters on cmdline, only arguments. */
			goto extra_args;

		default:
			goto unknown;
		};
	}

	return 0;

extra_args:
	if (cl.type == CLIENT && !cl.msg.ticket.id[0]) {
		cparg(cl.msg.ticket.id, "ticket name");
	} else if (cl.type == GEOSTORE) {
		if (cl.op != ATTR_LIST) {
			cparg(cl.attr_msg.attr.name, "attribute name");
		}
		if (cl.op == ATTR_SET) {
			cparg(cl.attr_msg.attr.val, "attribute value");
		}
	}

	if (optind == argc)
		return 0;


	left = argc - optind;
	fprintf(stderr, "Superfluous argument%s: %s%s\n",
			left == 1 ? "" : "s",
			argv[optind],
			left == 1 ? "" : "...");
	exit(EXIT_FAILURE);

unknown:
	fprintf(stderr, "unknown option: %s\n", argv[optind]);
	exit(EXIT_FAILURE);

missingarg:
	fprintf(stderr, "not enough arguments\n");
	exit(EXIT_FAILURE);
}