bool cg_completion_timeout(void *fn, void *fnarg, int timeout)
{
	struct cg_completion *cgc;
	pthread_t pthread;
	bool ret = false;

	cgc = (struct cg_completion *)malloc(sizeof(struct cg_completion));
	if (unlikely(!cgc))
		return ret;
	cgsem_init(&cgc->cgsem);

#ifdef _MSC_VER
	cgc->fn = (void(__cdecl *)(void *))fn;
#else
	cgc->fn = fn;
#endif

	cgc->fnarg = fnarg;

	pthread_create(&pthread, NULL, completion_thread, (void *)cgc);

	ret = cgsem_mswait(&cgc->cgsem, timeout);

	if (ret)
		pthread_cancel(pthread);

	pthread_join(pthread, NULL);
	free(cgc);
	return !ret;
}
