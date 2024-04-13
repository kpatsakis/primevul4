release_files(struct iso9660 *iso9660)
{
	struct content *con, *connext;
	struct file_info *file;

	file = iso9660->use_files;
	while (file != NULL) {
		struct file_info *next = file->use_next;

		archive_string_free(&file->name);
		archive_string_free(&file->symlink);
		free(file->utf16be_name);
		con = file->contents.first;
		while (con != NULL) {
			connext = con->next;
			free(con);
			con = connext;
		}
		free(file);
		file = next;
	}
}
