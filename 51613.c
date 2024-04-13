re_add_entry(struct iso9660 *iso9660, struct file_info *file)
{
	file->re_next = NULL;
	*iso9660->re_files.last = file;
	iso9660->re_files.last = &(file->re_next);
}
