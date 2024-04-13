static int atl2_validate_option(int *value, struct atl2_option *opt)
{
	int i;
	struct atl2_opt_list *ent;

	if (*value == OPTION_UNSET) {
		*value = opt->def;
		return 0;
	}

	switch (opt->type) {
	case enable_option:
		switch (*value) {
		case OPTION_ENABLED:
			printk(KERN_INFO "%s Enabled\n", opt->name);
			return 0;
		case OPTION_DISABLED:
			printk(KERN_INFO "%s Disabled\n", opt->name);
			return 0;
		}
		break;
	case range_option:
		if (*value >= opt->arg.r.min && *value <= opt->arg.r.max) {
			printk(KERN_INFO "%s set to %i\n", opt->name, *value);
			return 0;
		}
		break;
	case list_option:
		for (i = 0; i < opt->arg.l.nr; i++) {
			ent = &opt->arg.l.p[i];
			if (*value == ent->i) {
				if (ent->str[0] != '\0')
					printk(KERN_INFO "%s\n", ent->str);
			return 0;
			}
		}
		break;
	default:
		BUG();
	}

	printk(KERN_INFO "Invalid %s specified (%i) %s\n",
		opt->name, *value, opt->err);
	*value = opt->def;
	return -1;
}
