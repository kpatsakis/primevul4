process_extended(u_int32_t id)
{
	char *request;
	int i, r;

	if ((r = sshbuf_get_cstring(iqueue, &request, NULL)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));
	for (i = 0; extended_handlers[i].handler != NULL; i++) {
		if (strcmp(request, extended_handlers[i].ext_name) == 0) {
			if (!request_permitted(&extended_handlers[i]))
				send_status(id, SSH2_FX_PERMISSION_DENIED);
			else
				extended_handlers[i].handler(id);
			break;
		}
	}
	if (extended_handlers[i].handler == NULL) {
		error("Unknown extended request \"%.100s\"", request);
		send_status(id, SSH2_FX_OP_UNSUPPORTED);	/* MUST */
	}
	free(request);
}
