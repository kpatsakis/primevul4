void sctp_write_space(struct sock *sk)
{
	struct sctp_association *asoc;
	struct list_head *pos;

	/* Wake up the tasks in each wait queue.  */
	list_for_each(pos, &((sctp_sk(sk))->ep->asocs)) {
		asoc = list_entry(pos, struct sctp_association, asocs);
		__sctp_write_space(asoc);
	}
}
