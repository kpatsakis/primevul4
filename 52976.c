int handle_revision_arg(const char *arg_, struct rev_info *revs, int flags, unsigned revarg_opt)
{
	struct object_context oc;
	char *dotdot;
	struct object *object;
	unsigned char sha1[20];
	int local_flags;
	const char *arg = arg_;
	int cant_be_filename = revarg_opt & REVARG_CANNOT_BE_FILENAME;
	unsigned get_sha1_flags = 0;

	flags = flags & UNINTERESTING ? flags | BOTTOM : flags & ~BOTTOM;

	dotdot = strstr(arg, "..");
	if (dotdot) {
		unsigned char from_sha1[20];
		const char *next = dotdot + 2;
		const char *this = arg;
		int symmetric = *next == '.';
		unsigned int flags_exclude = flags ^ (UNINTERESTING | BOTTOM);
		static const char head_by_default[] = "HEAD";
		unsigned int a_flags;

		*dotdot = 0;
		next += symmetric;

		if (!*next)
			next = head_by_default;
		if (dotdot == arg)
			this = head_by_default;
		if (this == head_by_default && next == head_by_default &&
		    !symmetric) {
			/*
			 * Just ".."?  That is not a range but the
			 * pathspec for the parent directory.
			 */
			if (!cant_be_filename) {
				*dotdot = '.';
				return -1;
			}
		}
		if (!get_sha1_committish(this, from_sha1) &&
		    !get_sha1_committish(next, sha1)) {
			struct object *a_obj, *b_obj;

			if (!cant_be_filename) {
				*dotdot = '.';
				verify_non_filename(revs->prefix, arg);
			}

			a_obj = parse_object(from_sha1);
			b_obj = parse_object(sha1);
			if (!a_obj || !b_obj) {
			missing:
				if (revs->ignore_missing)
					return 0;
				die(symmetric
				    ? "Invalid symmetric difference expression %s"
				    : "Invalid revision range %s", arg);
			}

			if (!symmetric) {
				/* just A..B */
				a_flags = flags_exclude;
			} else {
				/* A...B -- find merge bases between the two */
				struct commit *a, *b;
				struct commit_list *exclude;

				a = (a_obj->type == OBJ_COMMIT
				     ? (struct commit *)a_obj
				     : lookup_commit_reference(a_obj->sha1));
				b = (b_obj->type == OBJ_COMMIT
				     ? (struct commit *)b_obj
				     : lookup_commit_reference(b_obj->sha1));
				if (!a || !b)
					goto missing;
				exclude = get_merge_bases(a, b);
				add_rev_cmdline_list(revs, exclude,
						     REV_CMD_MERGE_BASE,
						     flags_exclude);
				add_pending_commit_list(revs, exclude,
							flags_exclude);
				free_commit_list(exclude);

				a_flags = flags | SYMMETRIC_LEFT;
			}

			a_obj->flags |= a_flags;
			b_obj->flags |= flags;
			add_rev_cmdline(revs, a_obj, this,
					REV_CMD_LEFT, a_flags);
			add_rev_cmdline(revs, b_obj, next,
					REV_CMD_RIGHT, flags);
			add_pending_object(revs, a_obj, this);
			add_pending_object(revs, b_obj, next);
			return 0;
		}
		*dotdot = '.';
	}
	dotdot = strstr(arg, "^@");
	if (dotdot && !dotdot[2]) {
		*dotdot = 0;
		if (add_parents_only(revs, arg, flags))
			return 0;
		*dotdot = '^';
	}
	dotdot = strstr(arg, "^!");
	if (dotdot && !dotdot[2]) {
		*dotdot = 0;
		if (!add_parents_only(revs, arg, flags ^ (UNINTERESTING | BOTTOM)))
			*dotdot = '^';
	}

	local_flags = 0;
	if (*arg == '^') {
		local_flags = UNINTERESTING | BOTTOM;
		arg++;
	}

	if (revarg_opt & REVARG_COMMITTISH)
		get_sha1_flags = GET_SHA1_COMMITTISH;

	if (get_sha1_with_context(arg, get_sha1_flags, sha1, &oc))
		return revs->ignore_missing ? 0 : -1;
	if (!cant_be_filename)
		verify_non_filename(revs->prefix, arg);
	object = get_reference(revs, arg, sha1, flags ^ local_flags);
	add_rev_cmdline(revs, object, arg_, REV_CMD_REV, flags ^ local_flags);
	add_pending_object_with_mode(revs, object, arg, oc.mode);
	return 0;
}
