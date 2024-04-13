xml2_read_toc(struct archive_read *a)
{
	xmlTextReaderPtr reader;
	struct xmlattr_list list;
	int r;

	reader = xmlReaderForIO(xml2_read_cb, xml2_close_cb, a, NULL, NULL, 0);
	if (reader == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Couldn't allocate memory for xml parser");
		return (ARCHIVE_FATAL);
	}
	xmlTextReaderSetErrorHandler(reader, xml2_error_hdr, a);

	while ((r = xmlTextReaderRead(reader)) == 1) {
		const char *name, *value;
		int type, empty;

		type = xmlTextReaderNodeType(reader);
		name = (const char *)xmlTextReaderConstLocalName(reader);
		switch (type) {
		case XML_READER_TYPE_ELEMENT:
			empty = xmlTextReaderIsEmptyElement(reader);
			r = xml2_xmlattr_setup(a, &list, reader);
			if (r == ARCHIVE_OK)
				r = xml_start(a, name, &list);
			xmlattr_cleanup(&list);
			if (r != ARCHIVE_OK)
				return (r);
			if (empty)
				xml_end(a, name);
			break;
		case XML_READER_TYPE_END_ELEMENT:
			xml_end(a, name);
			break;
		case XML_READER_TYPE_TEXT:
			value = (const char *)xmlTextReaderConstValue(reader);
			xml_data(a, value, strlen(value));
			break;
		case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
		default:
			break;
		}
		if (r < 0)
			break;
	}
	xmlFreeTextReader(reader);
	xmlCleanupParser();

	return ((r == 0)?ARCHIVE_OK:ARCHIVE_FATAL);
}
