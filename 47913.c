static bool recv_creds(int sock, struct ucred *cred, char *v)
{
	struct msghdr msg = { 0 };
	struct iovec iov;
	struct cmsghdr *cmsg;
	char cmsgbuf[CMSG_SPACE(sizeof(*cred))];
	char buf[1];
	int ret;
	int optval = 1;
	struct timeval tv;
	fd_set rfds;

	*v = '1';

	cred->pid = -1;
	cred->uid = -1;
	cred->gid = -1;

	if (setsockopt(sock, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) == -1) {
		fprintf(stderr, "Failed to set passcred: %s\n", strerror(errno));
		return false;
	}
	buf[0] = '1';
	if (write(sock, buf, 1) != 1) {
		fprintf(stderr, "Failed to start write on scm fd: %s\n", strerror(errno));
		return false;
	}

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_control = cmsgbuf;
	msg.msg_controllen = sizeof(cmsgbuf);

	iov.iov_base = buf;
	iov.iov_len = sizeof(buf);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	FD_ZERO(&rfds);
	FD_SET(sock, &rfds);
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	if (select(sock+1, &rfds, NULL, NULL, &tv) <= 0) {
		fprintf(stderr, "Failed to select for scm_cred: %s\n",
			  strerror(errno));
		return false;
	}
	ret = recvmsg(sock, &msg, MSG_DONTWAIT);
	if (ret < 0) {
		fprintf(stderr, "Failed to receive scm_cred: %s\n",
			  strerror(errno));
		return false;
	}

	cmsg = CMSG_FIRSTHDR(&msg);

	if (cmsg && cmsg->cmsg_len == CMSG_LEN(sizeof(struct ucred)) &&
			cmsg->cmsg_level == SOL_SOCKET &&
			cmsg->cmsg_type == SCM_CREDENTIALS) {
		memcpy(cred, CMSG_DATA(cmsg), sizeof(*cred));
	}
	*v = buf[0];

	return true;
}
