static int iwgif_init_screen(struct iwgifrcontext *rctx)
{
	struct iw_image *img;
	int bg_visible=0;
	int retval=0;

	if(rctx->screen_initialized) return 1;
	rctx->screen_initialized = 1;

	img = rctx->img;

	if(!rctx->include_screen) {
		rctx->screen_width = rctx->image_width;
		rctx->screen_height = rctx->image_height;
		rctx->image_left = 0;
		rctx->image_top = 0;
	}

	img->width = rctx->screen_width;
	img->height = rctx->screen_height;
	if(!iw_check_image_dimensions(rctx->ctx,img->width,img->height)) {
		return 0;
	}

	if(rctx->image_left>0 || rctx->image_top>0 ||
		(rctx->image_left+rctx->image_width < rctx->screen_width) ||
		(rctx->image_top+rctx->image_height < rctx->screen_height) )
	{
		bg_visible = 1;
	}

	if(rctx->has_transparency || bg_visible) {
		rctx->bytes_per_pixel=4;
		img->imgtype = IW_IMGTYPE_RGBA;
	}
	else {
		rctx->bytes_per_pixel=3;
		img->imgtype = IW_IMGTYPE_RGB;
	}
	img->bit_depth = 8;
	img->bpr = rctx->bytes_per_pixel * img->width;

	img->pixels = (iw_byte*)iw_malloc_large(rctx->ctx, img->bpr, img->height);
	if(!img->pixels) goto done;

	iw_zeromem(img->pixels,img->bpr*img->height);

	retval=1;
done:
	return retval;
}
