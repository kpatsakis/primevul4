get_page_size_area_prepared_cb (GdkPixbufLoader *loader,
				gpointer         data)
{
	gboolean *got_size = data;
	*got_size = TRUE;
}
