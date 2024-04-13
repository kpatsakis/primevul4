comics_document_save (EvDocument *document,
		      const char *uri,
		      GError    **error)
{
	ComicsDocument *comics_document = COMICS_DOCUMENT (document);

	return ev_xfer_uri_simple (comics_document->archive, uri, error);
}
