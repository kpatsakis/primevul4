int sock_cmsg_send(struct sock *sk, struct msghdr *msg,
		   struct sockcm_cookie *sockc)
{
	struct cmsghdr *cmsg;
	int ret;

	for_each_cmsghdr(cmsg, msg) {
		if (!CMSG_OK(msg, cmsg))
			return -EINVAL;
		if (cmsg->cmsg_level != SOL_SOCKET)
			continue;
		ret = __sock_cmsg_send(sk, msg, cmsg, sockc);
		if (ret)
			return ret;
	}
	return 0;
}
