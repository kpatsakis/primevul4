build_pathname(struct archive_string *as, struct file_info *file, int depth)
{
	if (depth > 1000) {
		return NULL;
	}
	if (file->parent != NULL && archive_strlen(&file->parent->name) > 0) {
		if (build_pathname(as, file->parent, depth + 1) == NULL) {
			return NULL;
		}
		archive_strcat(as, "/");
	}
	if (archive_strlen(&file->name) == 0)
		archive_strcat(as, ".");
	else
		archive_string_concat(as, &file->name);
	return (as->s);
}
