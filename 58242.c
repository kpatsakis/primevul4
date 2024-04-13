static int iwgif_read_graphic_control_ext(struct iwgifrcontext *rctx)
{
	int retval;

	if(!iwgif_read(rctx,rctx->rbuf,6)) goto done;

	if(rctx->rbuf[0]!=4) goto done;
	if(rctx->rbuf[5]!=0) goto done;
	rctx->has_transparency = (int)((rctx->rbuf[1])&0x01);
	if(rctx->has_transparency) {
		rctx->trans_color_index = (int)rctx->rbuf[4];
	}

	retval=1;
done:
	return retval;
}
