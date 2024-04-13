static void unlink_lockfile(int fd)
{
	unlink(cl.lockfile);
	close(fd);
}