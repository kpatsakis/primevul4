static int parse_options(char *options, struct iso9660_options *popt)
{
	char *p;
	int option;

	popt->map = 'n';
	popt->rock = 1;
	popt->joliet = 1;
	popt->cruft = 0;
	popt->hide = 0;
	popt->showassoc = 0;
	popt->check = 'u';		/* unset */
	popt->nocompress = 0;
	popt->blocksize = 1024;
	popt->fmode = popt->dmode = ISOFS_INVALID_MODE;
	popt->uid_set = 0;
	popt->gid_set = 0;
	popt->gid = GLOBAL_ROOT_GID;
	popt->uid = GLOBAL_ROOT_UID;
	popt->iocharset = NULL;
	popt->utf8 = 0;
	popt->overriderockperm = 0;
	popt->session=-1;
	popt->sbsector=-1;
	if (!options)
		return 1;

	while ((p = strsep(&options, ",")) != NULL) {
		int token;
		substring_t args[MAX_OPT_ARGS];
		unsigned n;

		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_norock:
			popt->rock = 0;
			break;
		case Opt_nojoliet:
			popt->joliet = 0;
			break;
		case Opt_hide:
			popt->hide = 1;
			break;
		case Opt_unhide:
		case Opt_showassoc:
			popt->showassoc = 1;
			break;
		case Opt_cruft:
			popt->cruft = 1;
			break;
		case Opt_utf8:
			popt->utf8 = 1;
			break;
#ifdef CONFIG_JOLIET
		case Opt_iocharset:
			popt->iocharset = match_strdup(&args[0]);
			break;
#endif
		case Opt_map_a:
			popt->map = 'a';
			break;
		case Opt_map_o:
			popt->map = 'o';
			break;
		case Opt_map_n:
			popt->map = 'n';
			break;
		case Opt_session:
			if (match_int(&args[0], &option))
				return 0;
			n = option;
			if (n > 99)
				return 0;
			popt->session = n + 1;
			break;
		case Opt_sb:
			if (match_int(&args[0], &option))
				return 0;
			popt->sbsector = option;
			break;
		case Opt_check_r:
			popt->check = 'r';
			break;
		case Opt_check_s:
			popt->check = 's';
			break;
		case Opt_ignore:
			break;
		case Opt_uid:
			if (match_int(&args[0], &option))
				return 0;
			popt->uid = make_kuid(current_user_ns(), option);
			if (!uid_valid(popt->uid))
				return 0;
			popt->uid_set = 1;
			break;
		case Opt_gid:
			if (match_int(&args[0], &option))
				return 0;
			popt->gid = make_kgid(current_user_ns(), option);
			if (!gid_valid(popt->gid))
				return 0;
			popt->gid_set = 1;
			break;
		case Opt_mode:
			if (match_int(&args[0], &option))
				return 0;
			popt->fmode = option;
			break;
		case Opt_dmode:
			if (match_int(&args[0], &option))
				return 0;
			popt->dmode = option;
			break;
		case Opt_overriderockperm:
			popt->overriderockperm = 1;
			break;
		case Opt_block:
			if (match_int(&args[0], &option))
				return 0;
			n = option;
			if (n != 512 && n != 1024 && n != 2048)
				return 0;
			popt->blocksize = n;
			break;
		case Opt_nocompress:
			popt->nocompress = 1;
			break;
		default:
			return 0;
		}
	}
	return 1;
}
