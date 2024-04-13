rede_add_entry(struct file_info *file)
{
	struct file_info *re;

	/*
	 * Find "RE" entry.
	 */
	re = file->parent;
	while (re != NULL && !re->re)
		re = re->parent;
	if (re == NULL)
		return (-1);

	file->re_next = NULL;
	*re->rede_files.last = file;
	re->rede_files.last = &(file->re_next);
	return (0);
}
