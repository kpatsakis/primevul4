static struct tcmur_handler *find_handler_by_subtype(gchar *subtype)
{
	struct tcmur_handler **handler;

	darray_foreach(handler, g_runner_handlers) {
		if (strcmp((*handler)->subtype, subtype) == 0)
			return *handler;
	}
	return NULL;
}
