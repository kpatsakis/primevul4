static void tq_freezethaw(struct thread_q *tq, bool frozen)
{
	mutex_lock(&tq->mutex);
	tq->frozen = frozen;
	pthread_cond_signal(&tq->cond);
	mutex_unlock(&tq->mutex);
}
