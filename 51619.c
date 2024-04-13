register_file(struct iso9660 *iso9660, struct file_info *file)
{

	file->use_next = iso9660->use_files;
	iso9660->use_files = file;
}
