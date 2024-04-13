user_setenv(argv)
    char **argv;
{
    char *arg = argv[0];
    char *eqp;
    struct userenv *uep, **insp;

    if ((eqp = strchr(arg, '=')) == NULL) {
	option_error("missing = in name=value: %s", arg);
	return 0;
    }
    if (eqp == arg) {
	option_error("missing variable name: %s", arg);
	return 0;
    }
    for (uep = userenv_list; uep != NULL; uep = uep->ue_next) {
	int nlen = strlen(uep->ue_name);
	if (nlen == (eqp - arg) &&
	    strncmp(arg, uep->ue_name, nlen) == 0)
	    break;
    }
    /* Ignore attempts by unprivileged users to override privileged sources */
    if (uep != NULL && !privileged_option && uep->ue_priv)
	return 1;
    /* The name never changes, so allocate it with the structure */
    if (uep == NULL) {
	uep = malloc(sizeof (*uep) + (eqp-arg));
	strncpy(uep->ue_name, arg, eqp-arg);
	uep->ue_name[eqp-arg] = '\0';
	uep->ue_next = NULL;
	insp = &userenv_list;
	while (*insp != NULL)
	    insp = &(*insp)->ue_next;
	*insp = uep;
    } else {
	struct userenv *uep2;
	for (uep2 = userenv_list; uep2 != NULL; uep2 = uep2->ue_next) {
	    if (uep2 != uep && !uep2->ue_isset)
		break;
	}
	if (uep2 == NULL && !uep->ue_isset)
	    find_option("unset")->flags |= OPT_NOPRINT;
	free(uep->ue_value);
    }
    uep->ue_isset = 1;
    uep->ue_priv = privileged_option;
    uep->ue_source = option_source;
    uep->ue_value = strdup(eqp + 1);
    curopt->flags &= ~OPT_NOPRINT;
    return 1;
}
