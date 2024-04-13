static int iwgif_read_color_table(struct iwgifrcontext *rctx, struct iw_palette *ct)
{
	int i;
	if(ct->num_entries<1) return 1;

	if(!iwgif_read(rctx,rctx->rbuf,3*ct->num_entries)) return 0;
	for(i=0;i<ct->num_entries;i++) {
		ct->entry[i].r = rctx->rbuf[3*i+0];
		ct->entry[i].g = rctx->rbuf[3*i+1];
		ct->entry[i].b = rctx->rbuf[3*i+2];
	}
	return 1;
}
