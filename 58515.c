static int nfsd_startup_generic(int nrservs)
{
	int ret;

	if (nfsd_users++)
		return 0;

	/*
	 * Readahead param cache - will no-op if it already exists.
	 * (Note therefore results will be suboptimal if number of
	 * threads is modified after nfsd start.)
	 */
	ret = nfsd_racache_init(2*nrservs);
	if (ret)
		goto dec_users;

	ret = nfs4_state_start();
	if (ret)
		goto out_racache;
	return 0;

out_racache:
	nfsd_racache_shutdown();
dec_users:
	nfsd_users--;
	return ret;
}
