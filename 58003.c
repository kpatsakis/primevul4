vbf_stp_fetchbody(struct worker *wrk, struct busyobj *bo)
{
	ssize_t l;
	uint8_t *ptr;
	enum vfp_status vfps = VFP_ERROR;
	ssize_t est;
	struct vfp_ctx *vfc;

	CHECK_OBJ_NOTNULL(bo, BUSYOBJ_MAGIC);
	vfc = bo->vfc;
	CHECK_OBJ_NOTNULL(vfc, VFP_CTX_MAGIC);

	AN(vfc->vfp_nxt);

	est = bo->htc->content_length;
	if (est < 0)
		est = 0;

	do {
		if (vfc->oc->flags & OC_F_ABANDON) {
			/*
			 * A pass object and delivery was terminated
			 * We don't fail the fetch, in order for hit-for-pass
			 * objects to be created.
			 */
			AN(vfc->oc->flags & OC_F_PASS);
			VSLb(wrk->vsl, SLT_FetchError,
			    "Pass delivery abandoned");
			bo->htc->doclose = SC_RX_BODY;
			break;
		}
		AZ(vfc->failed);
		l = est;
		assert(l >= 0);
		if (VFP_GetStorage(vfc, &l, &ptr) != VFP_OK) {
			bo->htc->doclose = SC_RX_BODY;
			break;
		}

		AZ(vfc->failed);
		vfps = VFP_Suck(vfc, ptr, &l);
		if (l > 0 && vfps != VFP_ERROR) {
			bo->acct.beresp_bodybytes += l;
			VFP_Extend(vfc, l);
			if (est >= l)
				est -= l;
			else
				est = 0;
		}
	} while (vfps == VFP_OK);

	if (vfc->failed) {
		(void)VFP_Error(vfc, "Fetch pipeline failed to process");
		bo->htc->doclose = SC_RX_BODY;
		VFP_Close(vfc);
		VDI_Finish(wrk, bo);
		if (!bo->do_stream) {
			assert(bo->fetch_objcore->boc->state < BOS_STREAM);
			return (F_STP_ERROR);
		} else {
			wrk->stats->fetch_failed++;
			return (F_STP_FAIL);
		}
	}

	ObjTrimStore(wrk, vfc->oc);
	return (F_STP_FETCHEND);
}
