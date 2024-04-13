comics_document_class_init (ComicsDocumentClass *klass)
{
	GObjectClass    *gobject_class = G_OBJECT_CLASS (klass);
	EvDocumentClass *ev_document_class = EV_DOCUMENT_CLASS (klass);

	gobject_class->finalize = comics_document_finalize;

	ev_document_class->load = comics_document_load;
	ev_document_class->save = comics_document_save;
	ev_document_class->get_n_pages = comics_document_get_n_pages;
	ev_document_class->get_page_size = comics_document_get_page_size;
	ev_document_class->render = comics_document_render;
}
