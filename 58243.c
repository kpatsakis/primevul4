static int iwgif_read_main(struct iwgifrcontext *rctx)
{
	int retval=0;
	int i;
	int image_found=0;

	for(i=0;i<256;i++) {
		rctx->colortable.entry[i].a=255;
	}

	if(!iwgif_read_file_header(rctx)) goto done;

	if(!iwgif_read_screen_descriptor(rctx)) goto done;

	if(!iwgif_read_color_table(rctx,&rctx->colortable)) goto done;

	if(rctx->has_bg_color) {
		iw_set_input_bkgd_label(rctx->ctx,
			((double)rctx->colortable.entry[rctx->bg_color_index].r)/255.0,
			((double)rctx->colortable.entry[rctx->bg_color_index].g)/255.0,
			((double)rctx->colortable.entry[rctx->bg_color_index].b)/255.0);
	}


	while(!image_found) {
		if(!iwgif_read(rctx,rctx->rbuf,1)) goto done;

		switch(rctx->rbuf[0]) {
		case 0x21: // extension
			if(!iwgif_read_extension(rctx)) goto done;
			break;
		case 0x2c: // image
			rctx->pages_seen++;
			if(rctx->page == rctx->pages_seen) {
				if(!iwgif_read_image(rctx)) goto done;
				image_found=1;
			}
			else {
				if(!iwgif_skip_image(rctx)) goto done;
			}
			break;
		case 0x3b: // file trailer
			if(rctx->pages_seen==0)
				iw_set_error(rctx->ctx,"No image in file");
			else
				iw_set_error(rctx->ctx,"Image not found");
			goto done;
		default:
			iw_set_error(rctx->ctx,"Invalid or unsupported GIF file");
			goto done;
		}
	}

	retval=1;

done:
	return retval;
}
