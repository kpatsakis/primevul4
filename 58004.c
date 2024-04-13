vbf_stp_fetchend(struct worker *wrk, struct busyobj *bo)
{

	AZ(bo->vfc->failed);
	VFP_Close(bo->vfc);

	AZ(ObjSetU64(wrk, bo->fetch_objcore, OA_LEN,
	    bo->fetch_objcore->boc->len_so_far));

	if (bo->do_stream)
		assert(bo->fetch_objcore->boc->state == BOS_STREAM);
	else {
		assert(bo->fetch_objcore->boc->state == BOS_REQ_DONE);
		HSH_Unbusy(wrk, bo->fetch_objcore);
	}

	/* Recycle the backend connection before setting BOS_FINISHED to
	   give predictable backend reuse behavior for varnishtest */
	VDI_Finish(bo->wrk, bo);

	ObjSetState(wrk, bo->fetch_objcore, BOS_FINISHED);
	VSLb_ts_busyobj(bo, "BerespBody", W_TIM_real(wrk));
	if (bo->stale_oc != NULL)
		HSH_Kill(bo->stale_oc);
	return (F_STP_DONE);
}
