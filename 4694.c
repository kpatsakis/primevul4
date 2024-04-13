int fmtutil_atari_decode_image(deark *c, struct atari_img_decode_data *adata)
{
	switch(adata->bpp) {
	case 16:
		return decode_atari_image_16(c, adata);
	case 8: case 4: case 2: case 1:
		return decode_atari_image_paletted(c, adata);
	}

	de_err(c, "Unsupported bits/pixel (%d)", (int)adata->bpp);
	return 0;
}