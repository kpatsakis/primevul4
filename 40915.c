void tcp_enter_memory_pressure(struct sock *sk)
{
	if (!tcp_memory_pressure) {
		NET_INC_STATS(sock_net(sk), LINUX_MIB_TCPMEMORYPRESSURES);
		tcp_memory_pressure = 1;
	}
}
