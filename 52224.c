cleanup(struct archive_read *a)
{
	struct mtree *mtree;
	struct mtree_entry *p, *q;

	mtree = (struct mtree *)(a->format->data);

	p = mtree->entries;
	while (p != NULL) {
		q = p->next;
		free(p->name);
		free_options(p->options);
		free(p);
		p = q;
	}
	archive_string_free(&mtree->line);
	archive_string_free(&mtree->current_dir);
	archive_string_free(&mtree->contents_name);
	archive_entry_linkresolver_free(mtree->resolver);

	free(mtree->buff);
	free(mtree);
	(a->format->data) = NULL;
	return (ARCHIVE_OK);
}
