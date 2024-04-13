static inline int vcc_writable(struct sock *sk)
{
	struct atm_vcc *vcc = atm_sk(sk);

	return (vcc->qos.txtp.max_sdu +
		atomic_read(&sk->sk_wmem_alloc)) <= sk->sk_sndbuf;
}
