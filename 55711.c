xmlattr_cleanup(struct xmlattr_list *list)
{
	struct xmlattr *attr, *next;

	attr = list->first;
	while (attr != NULL) {
		next = attr->next;
		free(attr->name);
		free(attr->value);
		free(attr);
		attr = next;
	}
	list->first = NULL;
	list->last = &(list->first);
}
