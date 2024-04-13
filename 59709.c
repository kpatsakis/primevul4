static void delete_x11_file(pid_t pid) {
	char *fname;
	if (asprintf(&fname, "%s/%d", RUN_FIREJAIL_X11_DIR, pid) == -1)
		errExit("asprintf");
	int rv = unlink(fname);
	(void) rv;
	free(fname);
}
