process_option(opt, cmd, argv)
    option_t *opt;
    char *cmd;
    char **argv;
{
    u_int32_t v;
    int iv, a;
    char *sv;
    int (*parser) __P((char **));
    int (*wildp) __P((char *, char **, int));
    char *optopt = (opt->type == o_wild)? "": " option";
    int prio = option_priority;
    option_t *mainopt = opt;

    current_option = opt->name;
    if ((opt->flags & OPT_PRIVFIX) && privileged_option)
	prio += OPRIO_ROOT;
    while (mainopt->flags & OPT_PRIOSUB)
	--mainopt;
    if (mainopt->flags & OPT_PRIO) {
	if (prio < mainopt->priority) {
	    /* new value doesn't override old */
	    if (prio == OPRIO_CMDLINE && mainopt->priority > OPRIO_ROOT) {
		option_error("%s%s set in %s cannot be overridden\n",
			     opt->name, optopt, mainopt->source);
		return 0;
	    }
	    return 1;
	}
	if (prio > OPRIO_ROOT && mainopt->priority == OPRIO_CMDLINE)
	    warn("%s%s from %s overrides command line",
		 opt->name, optopt, option_source);
    }

    if ((opt->flags & OPT_INITONLY) && phase != PHASE_INITIALIZE) {
	option_error("%s%s cannot be changed after initialization",
		     opt->name, optopt);
	return 0;
    }
    if ((opt->flags & OPT_PRIV) && !privileged_option) {
	option_error("using the %s%s requires root privilege",
		     opt->name, optopt);
	return 0;
    }
    if ((opt->flags & OPT_ENABLE) && *(bool *)(opt->addr2) == 0) {
	option_error("%s%s is disabled", opt->name, optopt);
	return 0;
    }
    if ((opt->flags & OPT_DEVEQUIV) && devnam_fixed) {
	option_error("the %s%s may not be changed in %s",
		     opt->name, optopt, option_source);
	return 0;
    }

    switch (opt->type) {
    case o_bool:
	v = opt->flags & OPT_VALUE;
	*(bool *)(opt->addr) = v;
	if (opt->addr2 && (opt->flags & OPT_A2COPY))
	    *(bool *)(opt->addr2) = v;
	else if (opt->addr2 && (opt->flags & OPT_A2CLR))
	    *(bool *)(opt->addr2) = 0;
	else if (opt->addr2 && (opt->flags & OPT_A2CLRB))
	    *(u_char *)(opt->addr2) &= ~v;
	else if (opt->addr2 && (opt->flags & OPT_A2OR))
	    *(u_char *)(opt->addr2) |= v;
	break;

    case o_int:
	iv = 0;
	if ((opt->flags & OPT_NOARG) == 0) {
	    if (!int_option(*argv, &iv))
		return 0;
	    if ((((opt->flags & OPT_LLIMIT) && iv < opt->lower_limit)
		 || ((opt->flags & OPT_ULIMIT) && iv > opt->upper_limit))
		&& !((opt->flags & OPT_ZEROOK && iv == 0))) {
		char *zok = (opt->flags & OPT_ZEROOK)? " zero or": "";
		switch (opt->flags & OPT_LIMITS) {
		case OPT_LLIMIT:
		    option_error("%s value must be%s >= %d",
				 opt->name, zok, opt->lower_limit);
		    break;
		case OPT_ULIMIT:
		    option_error("%s value must be%s <= %d",
				 opt->name, zok, opt->upper_limit);
		    break;
		case OPT_LIMITS:
		    option_error("%s value must be%s between %d and %d",
				opt->name, zok, opt->lower_limit, opt->upper_limit);
		    break;
		}
		return 0;
	    }
	}
	a = opt->flags & OPT_VALUE;
	if (a >= 128)
	    a -= 256;		/* sign extend */
	iv += a;
	if (opt->flags & OPT_INC)
	    iv += *(int *)(opt->addr);
	if ((opt->flags & OPT_NOINCR) && !privileged_option) {
	    int oldv = *(int *)(opt->addr);
	    if ((opt->flags & OPT_ZEROINF) ?
		(oldv != 0 && (iv == 0 || iv > oldv)) : (iv > oldv)) {
		option_error("%s value cannot be increased", opt->name);
		return 0;
	    }
	}
	*(int *)(opt->addr) = iv;
	if (opt->addr2 && (opt->flags & OPT_A2COPY))
	    *(int *)(opt->addr2) = iv;
	break;

    case o_uint32:
	if (opt->flags & OPT_NOARG) {
	    v = opt->flags & OPT_VALUE;
	    if (v & 0x80)
		    v |= 0xffffff00U;
	} else if (!number_option(*argv, &v, 16))
	    return 0;
	if (opt->flags & OPT_OR)
	    v |= *(u_int32_t *)(opt->addr);
	*(u_int32_t *)(opt->addr) = v;
	if (opt->addr2 && (opt->flags & OPT_A2COPY))
	    *(u_int32_t *)(opt->addr2) = v;
	break;

    case o_string:
	if (opt->flags & OPT_STATIC) {
	    strlcpy((char *)(opt->addr), *argv, opt->upper_limit);
	} else {
	    sv = strdup(*argv);
	    if (sv == NULL)
		novm("option argument");
	    *(char **)(opt->addr) = sv;
	}
	break;

    case o_special_noarg:
    case o_special:
	parser = (int (*) __P((char **))) opt->addr;
	curopt = opt;
	if (!(*parser)(argv))
	    return 0;
	if (opt->flags & OPT_A2LIST) {
	    struct option_value *ovp, *pp;

	    ovp = malloc(sizeof(*ovp) + strlen(*argv));
	    if (ovp != 0) {
		strcpy(ovp->value, *argv);
		ovp->source = option_source;
		ovp->next = NULL;
		if (opt->addr2 == NULL) {
		    opt->addr2 = ovp;
		} else {
		    for (pp = opt->addr2; pp->next != NULL; pp = pp->next)
			;
		    pp->next = ovp;
		}
	    }
	}
	break;

    case o_wild:
	wildp = (int (*) __P((char *, char **, int))) opt->addr;
	if (!(*wildp)(cmd, argv, 1))
	    return 0;
	break;
    }

    /*
     * If addr2 wasn't used by any flag (OPT_A2COPY, etc.) but is set,
     * treat it as a bool and set/clear it based on the OPT_A2CLR bit.
     */
    if (opt->addr2 && (opt->flags & (OPT_A2COPY|OPT_ENABLE
		|OPT_A2PRINTER|OPT_A2STRVAL|OPT_A2LIST|OPT_A2OR)) == 0)
	*(bool *)(opt->addr2) = !(opt->flags & OPT_A2CLR);

    mainopt->source = option_source;
    mainopt->priority = prio;
    mainopt->winner = opt - mainopt;

    return 1;
}
