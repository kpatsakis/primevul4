rede_get_entry(struct file_info *re)
{
	struct file_info *file;

	if ((file = re->rede_files.first) != NULL) {
		re->rede_files.first = file->re_next;
		if (re->rede_files.first == NULL)
			re->rede_files.last =
			    &(re->rede_files.first);
	}
	return (file);
}
