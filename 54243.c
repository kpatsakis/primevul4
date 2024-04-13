proxy_error (int fd, char *msg)
{
	write (fd, "0\n", 2);
	write (fd, msg, strlen (msg));
}
