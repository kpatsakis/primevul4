free_Header(struct _7z_header_info *h)
{
	free(h->emptyStreamBools);
	free(h->emptyFileBools);
	free(h->antiBools);
	free(h->attrBools);
}
