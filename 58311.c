static void feh_wm_set_bg_centered(Pixmap pmap, Imlib_Image im, int use_filelist,
		int x, int y, int w, int h)
{
	int offset_x, offset_y;

	if (use_filelist)
		feh_wm_load_next(&im);

	if(opt.geom_flags & XValue)
		if(opt.geom_flags & XNegative)
			offset_x = (w - gib_imlib_image_get_width(im)) + opt.geom_x;
		else
			offset_x = opt.geom_x;
	else
		offset_x = (w - gib_imlib_image_get_width(im)) >> 1;

	if(opt.geom_flags & YValue)
		if(opt.geom_flags & YNegative)
			offset_y = (h - gib_imlib_image_get_height(im)) + opt.geom_y;
		else
			offset_y = opt.geom_y;
	else
		offset_y = (h - gib_imlib_image_get_height(im)) >> 1;

	gib_imlib_render_image_part_on_drawable_at_size(pmap, im,
		((offset_x < 0) ? -offset_x : 0),
		((offset_y < 0) ? -offset_y : 0),
		w,
		h,
		x + ((offset_x > 0) ? offset_x : 0),
		y + ((offset_y > 0) ? offset_y : 0),
		w,
		h,
		1, 0, 0);

	if (use_filelist)
		gib_imlib_free_image_and_decache(im);

	return;
}
