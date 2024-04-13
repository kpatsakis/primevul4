static int decode_atari_image_paletted(deark *c, struct atari_img_decode_data *adata)
{
	i64 i, j;
	i64 plane;
	i64 rowspan;
	u8 b;
	u32 v;
	i64 planespan;
	i64 ncolors;

	planespan = 2*((adata->w+15)/16);
	rowspan = planespan*adata->bpp;
	if(adata->ncolors>0)
		ncolors = adata->ncolors;
	else
		ncolors = ((i64)1)<<adata->bpp;

	for(j=0; j<adata->h; j++) {
		for(i=0; i<adata->w; i++) {
			v = 0;

			for(plane=0; plane<adata->bpp; plane++) {
				if(adata->was_compressed==0) {
					// TODO: Simplify this.
					if(adata->bpp==1) {
						b = de_get_bits_symbol(adata->unc_pixels, 1, j*rowspan, i);
					}
					else if(adata->bpp==2) {
						b = de_get_bits_symbol(adata->unc_pixels, 1,
							j*rowspan + 2*plane + (i/16)*2, i);
					}
					else if(adata->bpp==4) {
						b = de_get_bits_symbol(adata->unc_pixels, 1,
							j*rowspan + 2*plane + (i/2-(i/2)%16)+8*((i%32)/16), i%16);
					}
					else if(adata->bpp==8) {
						b = de_get_bits_symbol(adata->unc_pixels, 1,
							j*rowspan + 2*plane + (i-i%16), i%16);
					}
					else {
						b = 0;
					}
				}
				else {
					b = de_get_bits_symbol(adata->unc_pixels, 1, j*rowspan + plane*planespan, i);
				}
				if(b) v |= 1<<plane;
			}

			if(adata->is_spectrum512) {
				v = spectrum512_FindIndex(i, v);
				if(j>0) {
					v += (unsigned int)(48*(j));
				}
			}
			if(v>=(unsigned int)ncolors) v=(unsigned int)(ncolors-1);

			de_bitmap_setpixel_rgb(adata->img, i, j, adata->pal[v]);
		}
	}
	return 1;
}