int sock_diag_put_meminfo(struct sock *sk, struct sk_buff *skb, int attrtype)
{
	u32 mem[SK_MEMINFO_VARS];

	mem[SK_MEMINFO_RMEM_ALLOC] = sk_rmem_alloc_get(sk);
	mem[SK_MEMINFO_RCVBUF] = sk->sk_rcvbuf;
	mem[SK_MEMINFO_WMEM_ALLOC] = sk_wmem_alloc_get(sk);
	mem[SK_MEMINFO_SNDBUF] = sk->sk_sndbuf;
	mem[SK_MEMINFO_FWD_ALLOC] = sk->sk_forward_alloc;
	mem[SK_MEMINFO_WMEM_QUEUED] = sk->sk_wmem_queued;
	mem[SK_MEMINFO_OPTMEM] = atomic_read(&sk->sk_omem_alloc);
	mem[SK_MEMINFO_BACKLOG] = sk->sk_backlog.len;

	return nla_put(skb, attrtype, sizeof(mem), &mem);
}
