static void vcc_sock_destruct(struct sock *sk)
{
	if (atomic_read(&sk->sk_rmem_alloc))
		printk(KERN_DEBUG "%s: rmem leakage (%d bytes) detected.\n",
		       __func__, atomic_read(&sk->sk_rmem_alloc));

	if (atomic_read(&sk->sk_wmem_alloc))
		printk(KERN_DEBUG "%s: wmem leakage (%d bytes) detected.\n",
		       __func__, atomic_read(&sk->sk_wmem_alloc));
}
