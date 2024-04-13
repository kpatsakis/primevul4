IW_IMPL(int) iw_imgtype_num_channels(int t)
{
	switch(t) {
	case IW_IMGTYPE_RGBA:
		return 4;
	case IW_IMGTYPE_RGB:
		return 3;
	case IW_IMGTYPE_GRAYA:
		return 2;
	}
	return 1;
}
