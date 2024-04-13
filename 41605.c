static inline void __sctp_put_port(struct sock *sk)
{
	struct sctp_bind_hashbucket *head =
		&sctp_port_hashtable[sctp_phashfn(sock_net(sk),
						  inet_sk(sk)->inet_num)];
	struct sctp_bind_bucket *pp;

	sctp_spin_lock(&head->lock);
	pp = sctp_sk(sk)->bind_hash;
	__sk_del_bind_node(sk);
	sctp_sk(sk)->bind_hash = NULL;
	inet_sk(sk)->inet_num = 0;
	sctp_bucket_destroy(pp);
	sctp_spin_unlock(&head->lock);
}
