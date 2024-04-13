comics_remove_dir (gchar *path_name) 
{
	GDir  *content_dir;
	const gchar *filename;
	gchar *filename_with_path;
	
	if (g_file_test (path_name, G_FILE_TEST_IS_DIR)) {
		content_dir = g_dir_open  (path_name, 0, NULL);
		filename  = g_dir_read_name (content_dir);
		while (filename) {
			filename_with_path = 
				g_build_filename (path_name, 
						  filename, NULL);
			comics_remove_dir (filename_with_path);
			g_free (filename_with_path);
			filename = g_dir_read_name (content_dir);
		}
		g_dir_close (content_dir);
	}
	/* Note from g_remove() documentation: on Windows, it is in general not 
	 * possible to remove a file that is open to some process, or mapped 
	 * into memory.*/
	return (g_remove (path_name));
}
