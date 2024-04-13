static void iwgif_record_pixel(struct iwgifrcontext *rctx, unsigned int coloridx,
		int offset)
{
	struct iw_image *img;
	unsigned int r,g,b,a;
	size_t pixnum;
	size_t xi,yi; // position in image coordinates
	size_t xs,ys; // position in screen coordinates
	iw_byte *ptr;

	img = rctx->img;


	pixnum = rctx->pixels_set + offset;
	xi = pixnum%rctx->image_width;
	yi = pixnum/rctx->image_width;
	xs = rctx->image_left + xi;
	ys = rctx->image_top + yi;

	if(yi>=(size_t)rctx->image_height) return;
	if(xs>=(size_t)rctx->screen_width) return;
	if(ys>=(size_t)rctx->screen_height) return;

	if(rctx->row_pointers[yi]==NULL) return;


	if(coloridx<(unsigned int)rctx->colortable.num_entries) {
		r=rctx->colortable.entry[coloridx].r;
		g=rctx->colortable.entry[coloridx].g;
		b=rctx->colortable.entry[coloridx].b;
		a=rctx->colortable.entry[coloridx].a;
	}
	else {
		return; // Illegal palette index
	}


	ptr = &rctx->row_pointers[yi][rctx->bytes_per_pixel*xi];
	ptr[0]=r; ptr[1]=g; ptr[2]=b;
	if(img->imgtype==IW_IMGTYPE_RGBA) {
		ptr[3]=a;
	}
}
