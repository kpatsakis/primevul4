static void option_export_marks(const char *marks)
{
	export_marks_file = make_fast_import_path(marks);
	safe_create_leading_directories_const(export_marks_file);
}
