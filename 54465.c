bool tcmur_unregister_handler(struct tcmur_handler *handler)
{
	int i;
	for (i = 0; i < darray_size(g_runner_handlers); i++) {
		if (darray_item(g_runner_handlers, i) == handler) {
			darray_remove(g_runner_handlers, i);
			return true;
		}
	}
	return false;
}
