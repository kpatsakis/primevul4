static int vsock_auto_bind(struct vsock_sock *vsk)
{
	struct sock *sk = sk_vsock(vsk);
	struct sockaddr_vm local_addr;

	if (vsock_addr_bound(&vsk->local_addr))
		return 0;
	vsock_addr_init(&local_addr, VMADDR_CID_ANY, VMADDR_PORT_ANY);
	return __vsock_bind(sk, &local_addr);
}
