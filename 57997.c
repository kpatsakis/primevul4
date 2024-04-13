vbf_allocobj(struct busyobj *bo, unsigned l)
{
	struct objcore *oc;
	const struct stevedore *stv;
	double lifetime;

	CHECK_OBJ_NOTNULL(bo, BUSYOBJ_MAGIC);
	oc = bo->fetch_objcore;
	CHECK_OBJ_NOTNULL(oc, OBJCORE_MAGIC);

	lifetime = oc->ttl + oc->grace + oc->keep;

	if (bo->uncacheable || lifetime < cache_param->shortlived)
		stv = stv_transient;
	else
		stv = bo->storage;

	bo->storage = NULL;
	bo->storage_hint = NULL;

	if (stv == NULL)
		return (0);

	if (STV_NewObject(bo->wrk, bo->fetch_objcore, stv, l))
		return (1);

	if (stv == stv_transient)
		return (0);

	/*
	 * Try to salvage the transaction by allocating a shortlived object
	 * on Transient storage.
	 */

	if (oc->ttl > cache_param->shortlived)
		oc->ttl = cache_param->shortlived;
	oc->grace = 0.0;
	oc->keep = 0.0;
	return (STV_NewObject(bo->wrk, bo->fetch_objcore, stv_transient, l));
}
