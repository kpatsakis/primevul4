static int iwgif_skip_image(struct iwgifrcontext *rctx)
{
	int has_local_ct;
	int local_ct_size;
	int ct_num_entries;
	int retval=0;

	if(!iwgif_read(rctx,rctx->rbuf,9)) goto done;

	has_local_ct = (int)((rctx->rbuf[8]>>7)&0x01);
	if(has_local_ct) {
		local_ct_size = (int)(rctx->rbuf[8]&0x07);
		ct_num_entries = 1<<(1+local_ct_size);
	}

	if(has_local_ct) {
		if(!iwgif_read(rctx,rctx->rbuf,3*ct_num_entries)) goto done;
	}

	if(!iwgif_read(rctx,rctx->rbuf,1)) goto done;

	if(!iwgif_skip_subblocks(rctx)) goto done;

	rctx->has_transparency = 0;

	retval=1;

done:
	return retval;
}
