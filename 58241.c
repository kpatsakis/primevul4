static int iwgif_read_file_header(struct iwgifrcontext *rctx)
{
	if(!iwgif_read(rctx,rctx->rbuf,6)) return 0;
	if(rctx->rbuf[0]!='G' || rctx->rbuf[1]!='I' || rctx->rbuf[2]!='F') {
		iw_set_error(rctx->ctx,"Not a GIF file");
		return 0;
	}
	return 1;
}
