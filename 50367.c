static bool proc_sys_link_fill_cache(struct file *file,
				    struct dir_context *ctx,
				    struct ctl_table_header *head,
				    struct ctl_table *table)
{
	bool ret = true;
	head = sysctl_head_grab(head);

	if (S_ISLNK(table->mode)) {
		/* It is not an error if we can not follow the link ignore it */
		int err = sysctl_follow_link(&head, &table);
		if (err)
			goto out;
	}

	ret = proc_sys_fill_cache(file, ctx, head, table);
out:
	sysctl_head_finish(head);
	return ret;
}
