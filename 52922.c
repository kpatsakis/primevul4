static void option_import_marks(const char *marks,
					int from_stream, int ignore_missing)
{
	if (import_marks_file) {
		if (from_stream)
			die("Only one import-marks command allowed per stream");

		/* read previous mark file */
		if(!import_marks_file_from_stream)
			read_marks();
	}

	import_marks_file = make_fast_import_path(marks);
	safe_create_leading_directories_const(import_marks_file);
	import_marks_file_from_stream = from_stream;
	import_marks_file_ignore_missing = ignore_missing;
}
