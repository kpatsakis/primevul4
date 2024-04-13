xml2_xmlattr_setup(struct archive_read *a,
    struct xmlattr_list *list, xmlTextReaderPtr reader)
{
	struct xmlattr *attr;
	int r;

	list->first = NULL;
	list->last = &(list->first);
	r = xmlTextReaderMoveToFirstAttribute(reader);
	while (r == 1) {
		attr = malloc(sizeof*(attr));
		if (attr == NULL) {
			archive_set_error(&a->archive, ENOMEM, "Out of memory");
			return (ARCHIVE_FATAL);
		}
		attr->name = strdup(
		    (const char *)xmlTextReaderConstLocalName(reader));
		if (attr->name == NULL) {
			free(attr);
			archive_set_error(&a->archive, ENOMEM, "Out of memory");
			return (ARCHIVE_FATAL);
		}
		attr->value = strdup(
		    (const char *)xmlTextReaderConstValue(reader));
		if (attr->value == NULL) {
			free(attr->name);
			free(attr);
			archive_set_error(&a->archive, ENOMEM, "Out of memory");
			return (ARCHIVE_FATAL);
		}
		attr->next = NULL;
		*list->last = attr;
		list->last = &(attr->next);
		r = xmlTextReaderMoveToNextAttribute(reader);
	}
	return (r);
}
