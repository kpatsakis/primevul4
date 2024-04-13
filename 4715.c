void fmtutil_atari_set_standard_density(deark *c, struct atari_img_decode_data *adata,
	de_finfo *fi)
{
	switch(adata->bpp) {
	case 4:
		fi->density.code = DE_DENSITY_UNK_UNITS;
		fi->density.xdens = 240.0;
		fi->density.ydens = 200.0;
		break;
	case 2:
		fi->density.code = DE_DENSITY_UNK_UNITS;
		fi->density.xdens = 480.0;
		fi->density.ydens = 200.0;
		break;
	case 1:
		fi->density.code = DE_DENSITY_UNK_UNITS;
		fi->density.xdens = 480.0;
		fi->density.ydens = 400.0;
		break;
	}
}