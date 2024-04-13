IW_IMPL(void) iw_get_output_image(struct iw_context *ctx, struct iw_image *img)
{
	int k;

	iw_zeromem(img,sizeof(struct iw_image));
	img->width = ctx->optctx.width;
	img->height = ctx->optctx.height;
	img->imgtype = ctx->optctx.imgtype;
	img->sampletype = ctx->img2.sampletype;
	img->bit_depth = ctx->optctx.bit_depth;
	img->pixels = (iw_byte*)ctx->optctx.pixelsptr;
	img->bpr = ctx->optctx.bpr;
	img->density_code = ctx->img2.density_code;
	img->density_x = ctx->img2.density_x;
	img->density_y = ctx->img2.density_y;
	img->rendering_intent = ctx->img2.rendering_intent;

	img->has_bkgdlabel = ctx->optctx.has_bkgdlabel;
	for(k=0;k<4;k++) {
		if(ctx->optctx.bit_depth==8) {
			img->bkgdlabel.c[k] = ((double)ctx->optctx.bkgdlabel[k])/255.0;
		}
		else {
			img->bkgdlabel.c[k] = ((double)ctx->optctx.bkgdlabel[k])/65535.0;
		}
	}

	img->has_colorkey_trns = ctx->optctx.has_colorkey_trns;
	img->colorkey[0] = ctx->optctx.colorkey[0];
	img->colorkey[1] = ctx->optctx.colorkey[1];
	img->colorkey[2] = ctx->optctx.colorkey[2];
	if(ctx->reduced_output_maxcolor_flag) {
		img->reduced_maxcolors = 1;
		if(IW_IMGTYPE_IS_GRAY(img->imgtype)) {
			img->maxcolorcode[IW_CHANNELTYPE_GRAY] = ctx->img2_ci[0].maxcolorcode_int;
			if(IW_IMGTYPE_HAS_ALPHA(img->imgtype)) {
				img->maxcolorcode[IW_CHANNELTYPE_ALPHA] = ctx->img2_ci[1].maxcolorcode_int;
			}
		}
		else {
			img->maxcolorcode[IW_CHANNELTYPE_RED]   = ctx->img2_ci[0].maxcolorcode_int;
			img->maxcolorcode[IW_CHANNELTYPE_GREEN] = ctx->img2_ci[1].maxcolorcode_int;
			img->maxcolorcode[IW_CHANNELTYPE_BLUE]  = ctx->img2_ci[2].maxcolorcode_int;
			if(IW_IMGTYPE_HAS_ALPHA(img->imgtype)) {
				img->maxcolorcode[IW_CHANNELTYPE_ALPHA] = ctx->img2_ci[3].maxcolorcode_int;
			}
		}
	}
}
