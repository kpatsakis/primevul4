vbf_stp_condfetch(struct worker *wrk, struct busyobj *bo)
{

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(bo, BUSYOBJ_MAGIC);

	AZ(vbf_beresp2obj(bo));

	if (ObjHasAttr(bo->wrk, bo->stale_oc, OA_ESIDATA))
		AZ(ObjCopyAttr(bo->wrk, bo->fetch_objcore, bo->stale_oc,
		    OA_ESIDATA));

	AZ(ObjCopyAttr(bo->wrk, bo->fetch_objcore, bo->stale_oc, OA_FLAGS));
	AZ(ObjCopyAttr(bo->wrk, bo->fetch_objcore, bo->stale_oc, OA_GZIPBITS));

	if (bo->do_stream) {
		ObjSetState(wrk, bo->fetch_objcore, BOS_PREP_STREAM);
		HSH_Unbusy(wrk, bo->fetch_objcore);
		ObjSetState(wrk, bo->fetch_objcore, BOS_STREAM);
	}

	if (ObjIterate(wrk, bo->stale_oc, bo, vbf_objiterator, 0))
		(void)VFP_Error(bo->vfc, "Template object failed");

	if (bo->stale_oc->flags & OC_F_FAILED)
		(void)VFP_Error(bo->vfc, "Template object failed");
	if (bo->vfc->failed) {
		VDI_Finish(bo->wrk, bo);
		wrk->stats->fetch_failed++;
		return (F_STP_FAIL);
	}
	return (F_STP_FETCHEND);
}
