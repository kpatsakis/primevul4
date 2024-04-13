void sigterm_handler(int s) {
	g_hash_table_foreach(children, killchild, NULL);
	unlink(pidfname);

	exit(EXIT_SUCCESS);
}
