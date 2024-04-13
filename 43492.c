void *completion_thread(void *arg)
{
	struct cg_completion *cgc = (struct cg_completion *)arg;

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	cgc->fn(cgc->fnarg);
	cgsem_post(&cgc->cgsem);

	return NULL;
}
