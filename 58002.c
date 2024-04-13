vbf_stp_fetch(struct worker *wrk, struct busyobj *bo)
{
	const char *p;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(bo, BUSYOBJ_MAGIC);
	CHECK_OBJ_NOTNULL(bo->fetch_objcore, OBJCORE_MAGIC);

	assert(wrk->handling == VCL_RET_DELIVER);

	/*
	 * The VCL variables beresp.do_g[un]zip tells us how we want the
	 * object processed before it is stored.
	 *
	 * The backend Content-Encoding header tells us what we are going
	 * to receive, which we classify in the following three classes:
	 *
	 *	"Content-Encoding: gzip"	--> object is gzip'ed.
	 *	no Content-Encoding		--> object is not gzip'ed.
	 *	anything else			--> do nothing wrt gzip
	 *
	 */

	/* We do nothing unless the param is set */
	if (!cache_param->http_gzip_support)
		bo->do_gzip = bo->do_gunzip = 0;

	if (bo->htc->content_length == 0)
		http_Unset(bo->beresp, H_Content_Encoding);

	if (bo->htc->body_status != BS_NONE) {
		bo->is_gzip =
		    http_HdrIs(bo->beresp, H_Content_Encoding, "gzip");
		bo->is_gunzip =
		    !http_GetHdr(bo->beresp, H_Content_Encoding, NULL);
		assert(bo->is_gzip == 0 || bo->is_gunzip == 0);
	}

	/* We won't gunzip unless it is non-empty and gzip'ed */
	if (bo->htc->body_status == BS_NONE ||
	    bo->htc->content_length == 0 ||
	    (bo->do_gunzip && !bo->is_gzip))
		bo->do_gunzip = 0;

	/* We wont gzip unless it is non-empty and ungzip'ed */
	if (bo->htc->body_status == BS_NONE ||
	    bo->htc->content_length == 0 ||
	    (bo->do_gzip && !bo->is_gunzip))
		bo->do_gzip = 0;

	/* But we can't do both at the same time */
	assert(bo->do_gzip == 0 || bo->do_gunzip == 0);

	if (bo->do_gunzip || (bo->is_gzip && bo->do_esi))
		vbf_vfp_push(bo, &vfp_gunzip, 1);

	if (bo->htc->content_length != 0) {
		if (bo->do_esi && bo->do_gzip) {
			vbf_vfp_push(bo, &vfp_esi_gzip, 1);
		} else if (bo->do_esi && bo->is_gzip && !bo->do_gunzip) {
			vbf_vfp_push(bo, &vfp_esi_gzip, 1);
		} else if (bo->do_esi) {
			vbf_vfp_push(bo, &vfp_esi, 1);
		} else if (bo->do_gzip) {
			vbf_vfp_push(bo, &vfp_gzip, 1);
		} else if (bo->is_gzip && !bo->do_gunzip) {
			vbf_vfp_push(bo, &vfp_testgunzip, 1);
		}
	}

	if (bo->fetch_objcore->flags & OC_F_PRIVATE)
		AN(bo->uncacheable);

	/* No reason to try streaming a non-existing body */
	if (bo->htc->body_status == BS_NONE)
		bo->do_stream = 0;

	bo->fetch_objcore->boc->len_so_far = 0;

	if (VFP_Open(bo->vfc)) {
		(void)VFP_Error(bo->vfc, "Fetch pipeline failed to open");
		bo->htc->doclose = SC_RX_BODY;
		VDI_Finish(bo->wrk, bo);
		return (F_STP_ERROR);
	}

	if (vbf_beresp2obj(bo)) {
		(void)VFP_Error(bo->vfc, "Could not get storage");
		bo->htc->doclose = SC_RX_BODY;
		VFP_Close(bo->vfc);
		VDI_Finish(bo->wrk, bo);
		return (F_STP_ERROR);
	}

	if (bo->do_esi)
		ObjSetFlag(bo->wrk, bo->fetch_objcore, OF_ESIPROC, 1);

	if (bo->do_gzip || (bo->is_gzip && !bo->do_gunzip))
		ObjSetFlag(bo->wrk, bo->fetch_objcore, OF_GZIPED, 1);

	if (bo->do_gzip || bo->do_gunzip)
		ObjSetFlag(bo->wrk, bo->fetch_objcore, OF_CHGGZIP, 1);

	if (!(bo->fetch_objcore->flags & OC_F_PASS) &&
	    http_IsStatus(bo->beresp, 200) && (
	      http_GetHdr(bo->beresp, H_Last_Modified, &p) ||
	      http_GetHdr(bo->beresp, H_ETag, &p)))
		ObjSetFlag(bo->wrk, bo->fetch_objcore, OF_IMSCAND, 1);

	if (bo->htc->body_status != BS_NONE &&
	    VDI_GetBody(bo->wrk, bo) != 0) {
		(void)VFP_Error(bo->vfc,
		    "GetBody failed - workspace_backend overflow?");
		VFP_Close(bo->vfc);
		bo->htc->doclose = SC_OVERLOAD;
		VDI_Finish(bo->wrk, bo);
		return (F_STP_ERROR);
	}

	assert(bo->fetch_objcore->boc->refcount >= 1);

	assert(bo->fetch_objcore->boc->state == BOS_REQ_DONE);

	if (bo->do_stream) {
		ObjSetState(wrk, bo->fetch_objcore, BOS_PREP_STREAM);
		HSH_Unbusy(wrk, bo->fetch_objcore);
		ObjSetState(wrk, bo->fetch_objcore, BOS_STREAM);
	}

	VSLb(bo->vsl, SLT_Fetch_Body, "%u %s %s",
	    bo->htc->body_status, body_status_2str(bo->htc->body_status),
	    bo->do_stream ? "stream" : "-");

	if (bo->htc->body_status != BS_NONE) {
		assert(bo->htc->body_status != BS_ERROR);
		return (F_STP_FETCHBODY);
	}
	AZ(bo->vfc->failed);
	return (F_STP_FETCHEND);
}
