static int hidp_send_frame(struct socket *sock, unsigned char *data, int len)
{
	struct kvec iv = { data, len };
	struct msghdr msg;

	BT_DBG("sock %p data %p len %d", sock, data, len);

	if (!len)
		return 0;

	memset(&msg, 0, sizeof(msg));

	return kernel_sendmsg(sock, &msg, &iv, 1, len);
}
