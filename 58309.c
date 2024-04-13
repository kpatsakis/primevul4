static void feh_wm_load_next(Imlib_Image *im)
{
	static gib_list *wpfile = NULL;

	if (wpfile == NULL)
		wpfile = filelist;

	if (feh_load_image(im, FEH_FILE(wpfile->data)) == 0)
		eprintf("Unable to load image %s", FEH_FILE(wpfile->data)->filename);
	if (wpfile->next)
		wpfile = wpfile->next;

	return;
}
