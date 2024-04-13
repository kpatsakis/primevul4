static void sctp_enter_memory_pressure(struct sock *sk)
{
	sctp_memory_pressure = 1;
}
