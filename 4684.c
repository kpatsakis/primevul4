static int decode_atari_image_16(deark *c, struct atari_img_decode_data *adata)
{
	i64 i, j;
	i64 rowspan;
	u32 v;

	rowspan = adata->w * 2;

	for(j=0; j<adata->h; j++) {
		for(i=0; i<adata->w; i++) {
			v = (u32)dbuf_getu16be(adata->unc_pixels, j*rowspan + 2*i);
			v = de_rgb565_to_888(v);
			de_bitmap_setpixel_rgb(adata->img, i, j,v);
		}
	}
	return 1;
}