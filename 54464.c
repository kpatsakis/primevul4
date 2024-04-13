int tcmur_register_handler(struct tcmur_handler *handler)
{
	struct tcmur_handler *h;
	int i;

	for (i = 0; i < darray_size(g_runner_handlers); i++) {
		h = darray_item(g_runner_handlers, i);
		if (!strcmp(h->subtype, handler->subtype)) {
			tcmu_err("Handler %s has already been registered\n",
				 handler->subtype);
			return -1;
		}
	}

	darray_append(g_runner_handlers, handler);
	return 0;
}
