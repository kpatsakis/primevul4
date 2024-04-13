void tq_freeze(struct thread_q *tq)
{
	tq_freezethaw(tq, true);
}
