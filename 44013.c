print_option(opt, mainopt, printer, arg)
    option_t *opt, *mainopt;
    printer_func printer;
    void *arg;
{
	int i, v;
	char *p;

	if (opt->flags & OPT_NOPRINT)
		return;
	switch (opt->type) {
	case o_bool:
		v = opt->flags & OPT_VALUE;
		if (*(bool *)opt->addr != v)
			/* this can happen legitimately, e.g. lock
			   option turned off for default device */
			break;
		printer(arg, "%s", opt->name);
		break;
	case o_int:
		v = opt->flags & OPT_VALUE;
		if (v >= 128)
			v -= 256;
		i = *(int *)opt->addr;
		if (opt->flags & OPT_NOARG) {
			printer(arg, "%s", opt->name);
			if (i != v) {
				if (opt->flags & OPT_INC) {
					for (; i > v; i -= v)
						printer(arg, " %s", opt->name);
				} else
					printer(arg, " # oops: %d not %d\n",
						i, v);
			}
		} else {
			printer(arg, "%s %d", opt->name, i);
		}
		break;
	case o_uint32:
		printer(arg, "%s", opt->name);
		if ((opt->flags & OPT_NOARG) == 0)
			printer(arg, " %x", *(u_int32_t *)opt->addr);
		break;

	case o_string:
		if (opt->flags & OPT_HIDE) {
			p = "??????";
		} else {
			p = (char *) opt->addr;
			if ((opt->flags & OPT_STATIC) == 0)
				p = *(char **)p;
		}
		printer(arg, "%s %q", opt->name, p);
		break;

	case o_special:
	case o_special_noarg:
	case o_wild:
		if (opt->type != o_wild) {
			printer(arg, "%s", opt->name);
			if (n_arguments(opt) == 0)
				break;
			printer(arg, " ");
		}
		if (opt->flags & OPT_A2PRINTER) {
			void (*oprt) __P((option_t *, printer_func, void *));
			oprt = (void (*) __P((option_t *, printer_func,
					 void *)))opt->addr2;
			(*oprt)(opt, printer, arg);
		} else if (opt->flags & OPT_A2STRVAL) {
			p = (char *) opt->addr2;
			if ((opt->flags & OPT_STATIC) == 0)
				p = *(char **)p;
			printer("%q", p);
		} else if (opt->flags & OPT_A2LIST) {
			struct option_value *ovp;

			ovp = (struct option_value *) opt->addr2;
			for (;;) {
				printer(arg, "%q", ovp->value);
				if ((ovp = ovp->next) == NULL)
					break;
				printer(arg, "\t\t# (from %s)\n%s ",
					ovp->source, opt->name);
			}
		} else {
			printer(arg, "xxx # [don't know how to print value]");
		}
		break;

	default:
		printer(arg, "# %s value (type %d\?\?)", opt->name, opt->type);
		break;
	}
	printer(arg, "\t\t# (from %s)\n", mainopt->source);
}
