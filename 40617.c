static int pfkey_seq_show(struct seq_file *f, void *v)
{
	struct sock *s = sk_entry(v);

	if (v == SEQ_START_TOKEN)
		seq_printf(f ,"sk       RefCnt Rmem   Wmem   User   Inode\n");
	else
		seq_printf(f, "%pK %-6d %-6u %-6u %-6u %-6lu\n",
			       s,
			       atomic_read(&s->sk_refcnt),
			       sk_rmem_alloc_get(s),
			       sk_wmem_alloc_get(s),
			       from_kuid_munged(seq_user_ns(f), sock_i_uid(s)),
			       sock_i_ino(s)
			       );
	return 0;
}
