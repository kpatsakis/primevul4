cifs_uncached_readv_complete(struct work_struct *work)
{
	struct cifs_readdata *rdata = container_of(work,
						struct cifs_readdata, work);

	complete(&rdata->done);
	kref_put(&rdata->refcount, cifs_uncached_readdata_release);
}
