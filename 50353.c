static int insert_header(struct ctl_dir *dir, struct ctl_table_header *header)
{
	struct ctl_table *entry;
	int err;

	/* Is this a permanently empty directory? */
	if (is_empty_dir(&dir->header))
		return -EROFS;

	/* Am I creating a permanently empty directory? */
	if (header->ctl_table == sysctl_mount_point) {
		if (!RB_EMPTY_ROOT(&dir->root))
			return -EINVAL;
		set_empty_dir(dir);
	}

	dir->header.nreg++;
	header->parent = dir;
	err = insert_links(header);
	if (err)
		goto fail_links;
	for (entry = header->ctl_table; entry->procname; entry++) {
		err = insert_entry(header, entry);
		if (err)
			goto fail;
	}
	return 0;
fail:
	erase_header(header);
	put_links(header);
fail_links:
	if (header->ctl_table == sysctl_mount_point)
		clear_empty_dir(dir);
	header->parent = NULL;
	drop_sysctl_table(&dir->header);
	return err;
}
