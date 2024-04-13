void thr_info_cancel(struct thr_info *thr)
{
	if (!thr)
		return;

	if (PTH(thr) != 0L) {
		pthread_cancel(thr->pth);
		PTH(thr) = 0L;
	}
	cgsem_destroy(&thr->sem);
}
