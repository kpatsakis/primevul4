static int iwgif_make_row_pointers(struct iwgifrcontext *rctx)
{
	struct iw_image *img;
	int pass;
	int startrow, rowskip;
	int rowcount;
	int row;

	if(rctx->row_pointers) iw_free(rctx->ctx,rctx->row_pointers);
	rctx->row_pointers = (iw_byte**)iw_malloc(rctx->ctx, sizeof(iw_byte*)*rctx->image_height);
	if(!rctx->row_pointers) return 0;

	img = rctx->img;

	if(rctx->interlaced) {
		rowcount=0;
		for(pass=1;pass<=4;pass++) {
			if(pass==1) { startrow=0; rowskip=8; }
			else if(pass==2) { startrow=4; rowskip=8; }
			else if(pass==3) { startrow=2; rowskip=4; }
			else { startrow=1; rowskip=2; }

			for(row=startrow;row<rctx->image_height;row+=rowskip) {
				if(rctx->image_top+row < rctx->screen_height) {
					rctx->row_pointers[rowcount] = &img->pixels[(rctx->image_top+row)*img->bpr + (rctx->image_left)*rctx->bytes_per_pixel];
				}
				else {
					rctx->row_pointers[rowcount] = NULL;
				}
				rowcount++;
			}
		}
	}
	else {
		for(row=0;row<rctx->image_height;row++) {
			if(rctx->image_top+row < rctx->screen_height) {
				rctx->row_pointers[row] = &img->pixels[(rctx->image_top+row)*img->bpr + (rctx->image_left)*rctx->bytes_per_pixel];
			}
			else {
				rctx->row_pointers[row] = NULL;
			}
		}
	}
	return 1;
}
