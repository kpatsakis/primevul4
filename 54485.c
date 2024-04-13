comics_document_render (EvDocument      *document,
			EvRenderContext *rc)
{
	GdkPixbuf       *pixbuf;
	cairo_surface_t *surface;

	pixbuf = comics_document_render_pixbuf (document, rc);
	surface = ev_document_misc_surface_from_pixbuf (pixbuf);
	g_object_unref (pixbuf);
	
	return surface;
}
