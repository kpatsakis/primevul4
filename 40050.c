static int bt_seq_show(struct seq_file *seq, void *v)
{
	struct bt_seq_state *s = seq->private;
	struct bt_sock_list *l = s->l;

	if (v == SEQ_START_TOKEN) {
		seq_puts(seq ,"sk               RefCnt Rmem   Wmem   User   Inode  Src Dst Parent");

		if (l->custom_seq_show) {
			seq_putc(seq, ' ');
			l->custom_seq_show(seq, v);
		}

		seq_putc(seq, '\n');
	} else {
		struct sock *sk = sk_entry(v);
		struct bt_sock *bt = bt_sk(sk);

		seq_printf(seq,
			   "%pK %-6d %-6u %-6u %-6u %-6lu %pMR %pMR %-6lu",
			   sk,
			   atomic_read(&sk->sk_refcnt),
			   sk_rmem_alloc_get(sk),
			   sk_wmem_alloc_get(sk),
			   from_kuid(seq_user_ns(seq), sock_i_uid(sk)),
			   sock_i_ino(sk),
			   &bt->src,
			   &bt->dst,
			   bt->parent? sock_i_ino(bt->parent): 0LU);

		if (l->custom_seq_show) {
			seq_putc(seq, ' ');
			l->custom_seq_show(seq, v);
		}

		seq_putc(seq, '\n');
	}
	return 0;
}
