R_API void r_flag_list(RFlag *f, int rad, const char *pfx) {
	bool in_range = false;
	ut64 range_from = UT64_MAX;
	ut64 range_to = UT64_MAX;
	int fs = -1;
	RListIter *iter;
	RFlagItem *flag;
	if (rad == 'i') {
		char *sp, *arg = strdup (pfx + 1);
		sp = strchr (arg,  ' ');
		if (sp) {
			*sp++ = 0;
			range_from = r_num_math (f->num, arg);
			range_to = r_num_math (f->num, sp);
		} else {
			const int bsize = 4096;
			range_from = r_num_math (f->num, arg);
			range_to = range_from + bsize;
		}
		in_range = true;
		free (arg);
		rad = pfx[0];
		pfx = NULL;
	}

	if (pfx && !*pfx) {
		pfx = NULL;
	}

	switch (rad) {
	case 'j': {
		int first = 1;
		f->cb_printf ("[");
		r_list_foreach (f->flags, iter, flag) {
			if (IS_IN_SPACE (f, flag)) {
				continue;
			}
			if (in_range && (flag->offset < range_from || flag->offset >= range_to)) {
				continue;
			}
			f->cb_printf ("%s{\"name\":\"%s\",\"size\":%"PFMT64d",",
				first?"":",", flag->name, flag->size);
			if (flag->alias) {
				f->cb_printf ("\"alias\":\"%s\"", flag->alias);
			} else {
				f->cb_printf ("\"offset\":%"PFMT64d, flag->offset);
			}
			if (flag->comment)
				f->cb_printf (",\"comment\":\"}");
			else f->cb_printf ("}");
			first = 0;
		}
		f->cb_printf ("]\n");
		}
		break;
	case 1:
	case '*':
		r_list_foreach (f->flags, iter, flag) {
			if (IS_IN_SPACE (f, flag)) {
				continue;
			}
			if (in_range && (flag->offset < range_from || flag->offset >= range_to)) {
				continue;
			}
			if (fs == -1 || flag->space != fs) {
				const char *flagspace;
				fs = flag->space;
				flagspace = r_flag_space_get_i (f, fs);
				if (!flagspace || !*flagspace)
					flagspace = "*";
				f->cb_printf ("fs %s\n", flagspace);
			}
			if (flag->alias) {
				f->cb_printf ("fa %s %s\n", flag->name, flag->alias);
				if (flag->comment && *flag->comment)
					f->cb_printf ("\"fC %s %s\"\n",
						flag->name, flag->comment);
			} else {
				f->cb_printf ("f %s %"PFMT64d" 0x%08"PFMT64x"%s%s %s\n",
					flag->name, flag->size, flag->offset,
					pfx?"+":"", pfx?pfx:"",
					flag->comment? flag->comment:"");
			}
		}
		break;
	case 'n': // show original name
		r_list_foreach (f->flags, iter, flag) {
			if (IS_IN_SPACE (f, flag)) {
				continue;
			}
			if (in_range && (flag->offset < range_from || flag->offset >= range_to)) {
				continue;
			}
			if (flag->alias) {
				f->cb_printf ("%s %"PFMT64d" %s\n",
						flag->alias, flag->size, flag->realname);
			} else {
				f->cb_printf ("0x%08"PFMT64x" %"PFMT64d" %s\n",
						flag->offset, flag->size, flag->realname);
			}
		}
		break;
	default:
		r_list_foreach (f->flags, iter, flag) {
			if (IS_IN_SPACE (f, flag)) {
				continue;
			}
			if (in_range && (flag->offset < range_from || flag->offset >= range_to)) {
				continue;
			}
			if (flag->alias) {
				f->cb_printf ("%s %"PFMT64d" %s\n",
					flag->alias, flag->size, flag->name);
			} else {
				f->cb_printf ("0x%08"PFMT64x" %"PFMT64d" %s\n",
					flag->offset, flag->size, flag->name);
			}
		}
		break;
	}
