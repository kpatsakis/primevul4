static int recv_byte(int sockd)
{
	char c;

	if (recv(sockd, &c, 1, 0) != -1)
		return c;

	return -1;
}
