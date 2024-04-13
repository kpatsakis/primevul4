vbf_stp_fail(struct worker *wrk, const struct busyobj *bo)
{
	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(bo, BUSYOBJ_MAGIC);
	CHECK_OBJ_NOTNULL(bo->fetch_objcore, OBJCORE_MAGIC);

	assert(bo->fetch_objcore->boc->state < BOS_FINISHED);
	HSH_Fail(bo->fetch_objcore);
	if (!(bo->fetch_objcore->flags & OC_F_BUSY))
		HSH_Kill(bo->fetch_objcore);
	ObjSetState(wrk, bo->fetch_objcore, BOS_FAILED);
	return (F_STP_DONE);
}
