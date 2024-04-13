static int vsock_dgram_connect(struct socket *sock,
			       struct sockaddr *addr, int addr_len, int flags)
{
	int err;
	struct sock *sk;
	struct vsock_sock *vsk;
	struct sockaddr_vm *remote_addr;

	sk = sock->sk;
	vsk = vsock_sk(sk);

	err = vsock_addr_cast(addr, addr_len, &remote_addr);
	if (err == -EAFNOSUPPORT && remote_addr->svm_family == AF_UNSPEC) {
		lock_sock(sk);
		vsock_addr_init(&vsk->remote_addr, VMADDR_CID_ANY,
				VMADDR_PORT_ANY);
		sock->state = SS_UNCONNECTED;
		release_sock(sk);
		return 0;
	} else if (err != 0)
		return -EINVAL;

	lock_sock(sk);

	err = vsock_auto_bind(vsk);
	if (err)
		goto out;

	if (!transport->dgram_allow(remote_addr->svm_cid,
				    remote_addr->svm_port)) {
		err = -EINVAL;
		goto out;
	}

	memcpy(&vsk->remote_addr, remote_addr, sizeof(vsk->remote_addr));
	sock->state = SS_CONNECTED;

out:
	release_sock(sk);
	return err;
}
