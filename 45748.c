int tipc_sock_create_local(int type, struct socket **res)
{
	int rc;
	struct sock *sk;

	rc = sock_create_lite(AF_TIPC, type, 0, res);
	if (rc < 0) {
		pr_err("Failed to create kernel socket\n");
		return rc;
	}
	tipc_sk_create(&init_net, *res, 0, 1);

	sk = (*res)->sk;

	return 0;
}
