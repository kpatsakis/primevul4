static void sctp_v4_seq_dump_addr(struct seq_file *seq, union sctp_addr *addr)
{
	seq_printf(seq, "%pI4 ", &addr->v4.sin_addr);
}
