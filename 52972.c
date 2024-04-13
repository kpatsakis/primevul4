static void add_rev_cmdline_list(struct rev_info *revs,
				 struct commit_list *commit_list,
				 int whence,
				 unsigned flags)
{
	while (commit_list) {
		struct object *object = &commit_list->item->object;
		add_rev_cmdline(revs, object, sha1_to_hex(object->sha1),
				whence, flags);
		commit_list = commit_list->next;
	}
}
