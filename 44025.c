user_unsetenv(argv)
    char **argv;
{
    struct userenv *uep, **insp;
    char *arg = argv[0];

    if (strchr(arg, '=') != NULL) {
	option_error("unexpected = in name: %s", arg);
	return 0;
    }
    if (arg == '\0') {
	option_error("missing variable name for unset");
	return 0;
    }
    for (uep = userenv_list; uep != NULL; uep = uep->ue_next) {
	if (strcmp(arg, uep->ue_name) == 0)
	    break;
    }
    /* Ignore attempts by unprivileged users to override privileged sources */
    if (uep != NULL && !privileged_option && uep->ue_priv)
	return 1;
    /* The name never changes, so allocate it with the structure */
    if (uep == NULL) {
	uep = malloc(sizeof (*uep) + strlen(arg));
	strcpy(uep->ue_name, arg);
	uep->ue_next = NULL;
	insp = &userenv_list;
	while (*insp != NULL)
	    insp = &(*insp)->ue_next;
	*insp = uep;
    } else {
	struct userenv *uep2;
	for (uep2 = userenv_list; uep2 != NULL; uep2 = uep2->ue_next) {
	    if (uep2 != uep && uep2->ue_isset)
		break;
	}
	if (uep2 == NULL && uep->ue_isset)
	    find_option("set")->flags |= OPT_NOPRINT;
	free(uep->ue_value);
    }
    uep->ue_isset = 0;
    uep->ue_priv = privileged_option;
    uep->ue_source = option_source;
    uep->ue_value = NULL;
    curopt->flags &= ~OPT_NOPRINT;
    return 1;
}
