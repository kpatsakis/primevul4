static void server_exit(void)
{
	int rv;

	if (lock_fd >= 0) {
		/* We might not be able to delete it, but at least
		 * make it empty. */
		rv = ftruncate(lock_fd, 0);
		(void)rv;
		unlink_lockfile(lock_fd);
	}
	log_info("exiting");
}