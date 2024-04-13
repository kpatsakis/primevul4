skip(struct archive_read *a)
{
	struct mtree *mtree;

	mtree = (struct mtree *)(a->format->data);
	if (mtree->fd >= 0) {
		close(mtree->fd);
		mtree->fd = -1;
	}
	return (ARCHIVE_OK);
}
