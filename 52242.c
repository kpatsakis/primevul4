read_data(struct archive_read *a, const void **buff, size_t *size,
    int64_t *offset)
{
	size_t bytes_to_read;
	ssize_t bytes_read;
	struct mtree *mtree;

	mtree = (struct mtree *)(a->format->data);
	if (mtree->fd < 0) {
		*buff = NULL;
		*offset = 0;
		*size = 0;
		return (ARCHIVE_EOF);
	}
	if (mtree->buff == NULL) {
		mtree->buffsize = 64 * 1024;
		mtree->buff = malloc(mtree->buffsize);
		if (mtree->buff == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "Can't allocate memory");
			return (ARCHIVE_FATAL);
		}
	}

	*buff = mtree->buff;
	*offset = mtree->offset;
	if ((int64_t)mtree->buffsize > mtree->cur_size - mtree->offset)
		bytes_to_read = (size_t)(mtree->cur_size - mtree->offset);
	else
		bytes_to_read = mtree->buffsize;
	bytes_read = read(mtree->fd, mtree->buff, bytes_to_read);
	if (bytes_read < 0) {
		archive_set_error(&a->archive, errno, "Can't read");
		return (ARCHIVE_WARN);
	}
	if (bytes_read == 0) {
		*size = 0;
		return (ARCHIVE_EOF);
	}
	mtree->offset += bytes_read;
	*size = bytes_read;
	return (ARCHIVE_OK);
}
