cache_get_entry(struct iso9660 *iso9660)
{
	struct file_info *file;

	if ((file = iso9660->cache_files.first) != NULL) {
		iso9660->cache_files.first = file->next;
		if (iso9660->cache_files.first == NULL)
			iso9660->cache_files.last =
			    &(iso9660->cache_files.first);
	}
	return (file);
}
