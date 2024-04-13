perf_event_parse_addr_filter(struct perf_event *event, char *fstr,
			     struct list_head *filters)
{
	struct perf_addr_filter *filter = NULL;
	char *start, *orig, *filename = NULL;
	struct path path;
	substring_t args[MAX_OPT_ARGS];
	int state = IF_STATE_ACTION, token;
	unsigned int kernel = 0;
	int ret = -EINVAL;

	orig = fstr = kstrdup(fstr, GFP_KERNEL);
	if (!fstr)
		return -ENOMEM;

	while ((start = strsep(&fstr, " ,\n")) != NULL) {
		ret = -EINVAL;

		if (!*start)
			continue;

		/* filter definition begins */
		if (state == IF_STATE_ACTION) {
			filter = perf_addr_filter_new(event, filters);
			if (!filter)
				goto fail;
		}

		token = match_token(start, if_tokens, args);
		switch (token) {
		case IF_ACT_FILTER:
		case IF_ACT_START:
			filter->filter = 1;

		case IF_ACT_STOP:
			if (state != IF_STATE_ACTION)
				goto fail;

			state = IF_STATE_SOURCE;
			break;

		case IF_SRC_KERNELADDR:
		case IF_SRC_KERNEL:
			kernel = 1;

		case IF_SRC_FILEADDR:
		case IF_SRC_FILE:
			if (state != IF_STATE_SOURCE)
				goto fail;

			if (token == IF_SRC_FILE || token == IF_SRC_KERNEL)
				filter->range = 1;

			*args[0].to = 0;
			ret = kstrtoul(args[0].from, 0, &filter->offset);
			if (ret)
				goto fail;

			if (filter->range) {
				*args[1].to = 0;
				ret = kstrtoul(args[1].from, 0, &filter->size);
				if (ret)
					goto fail;
			}

			if (token == IF_SRC_FILE || token == IF_SRC_FILEADDR) {
				int fpos = filter->range ? 2 : 1;

				filename = match_strdup(&args[fpos]);
				if (!filename) {
					ret = -ENOMEM;
					goto fail;
				}
			}

			state = IF_STATE_END;
			break;

		default:
			goto fail;
		}

		/*
		 * Filter definition is fully parsed, validate and install it.
		 * Make sure that it doesn't contradict itself or the event's
		 * attribute.
		 */
		if (state == IF_STATE_END) {
			if (kernel && event->attr.exclude_kernel)
				goto fail;

			if (!kernel) {
				if (!filename)
					goto fail;

				/* look up the path and grab its inode */
				ret = kern_path(filename, LOOKUP_FOLLOW, &path);
				if (ret)
					goto fail_free_name;

				filter->inode = igrab(d_inode(path.dentry));
				path_put(&path);
				kfree(filename);
				filename = NULL;

				ret = -EINVAL;
				if (!filter->inode ||
				    !S_ISREG(filter->inode->i_mode))
					/* free_filters_list() will iput() */
					goto fail;
			}

			/* ready to consume more filters */
			state = IF_STATE_ACTION;
			filter = NULL;
		}
	}

	if (state != IF_STATE_ACTION)
		goto fail;

	kfree(orig);

	return 0;

fail_free_name:
	kfree(filename);
fail:
	free_filters_list(filters);
	kfree(orig);

	return ret;
}
