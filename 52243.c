read_header(struct archive_read *a, struct archive_entry *entry)
{
	struct mtree *mtree;
	char *p;
	int r, use_next;

	mtree = (struct mtree *)(a->format->data);

	if (mtree->fd >= 0) {
		close(mtree->fd);
		mtree->fd = -1;
	}

	if (mtree->entries == NULL) {
		mtree->resolver = archive_entry_linkresolver_new();
		if (mtree->resolver == NULL)
			return ARCHIVE_FATAL;
		archive_entry_linkresolver_set_strategy(mtree->resolver,
		    ARCHIVE_FORMAT_MTREE);
		r = read_mtree(a, mtree);
		if (r != ARCHIVE_OK)
			return (r);
	}

	a->archive.archive_format = mtree->archive_format;
	a->archive.archive_format_name = mtree->archive_format_name;

	for (;;) {
		if (mtree->this_entry == NULL)
			return (ARCHIVE_EOF);
		if (strcmp(mtree->this_entry->name, "..") == 0) {
			mtree->this_entry->used = 1;
			if (archive_strlen(&mtree->current_dir) > 0) {
				/* Roll back current path. */
				p = mtree->current_dir.s
				    + mtree->current_dir.length - 1;
				while (p >= mtree->current_dir.s && *p != '/')
					--p;
				if (p >= mtree->current_dir.s)
					--p;
				mtree->current_dir.length
				    = p - mtree->current_dir.s + 1;
			}
		}
		if (!mtree->this_entry->used) {
			use_next = 0;
			r = parse_file(a, entry, mtree, mtree->this_entry,
				&use_next);
			if (use_next == 0)
				return (r);
		}
		mtree->this_entry = mtree->this_entry->next;
	}
}
