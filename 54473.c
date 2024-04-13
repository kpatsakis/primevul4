static int parse_imagepath(char *cfgstring, gluster_server **hosts)
{
	gluster_server *entry = NULL;
	char *origp = strdup(cfgstring);
	char *p, *sep;

	if (!origp)
		goto fail;

	/* part before '@' is the volume name */
	p = origp;
	sep = strchr(p, '@');
	if (!sep)
		goto fail;

	*hosts = calloc(1, sizeof(gluster_server));
	if (!hosts)
                goto fail;
	entry = *hosts;

	entry->server = calloc(1, sizeof(gluster_hostdef));
	if (!entry->server)
                goto fail;

	*sep = '\0';
	entry->volname = strdup(p);
	if (!entry->volname)
		goto fail;

	/* part between '@' and 1st '/' is the server name */
	p = sep + 1;
	sep = strchr(p, '/');
	if (!sep)
		goto fail;

	*sep = '\0';
	entry->server->type = GLUSTER_TRANSPORT_TCP; /* FIXME: Get type dynamically */
	entry->server->u.inet.addr = strdup(p);
	if (!entry->server->u.inet.addr)
		goto fail;
	entry->server->u.inet.port = strdup(GLUSTER_PORT); /* FIXME: Get port dynamically */

	/* The rest is the path name */
	p = sep + 1;
	entry->path = strdup(p);
	if (!entry->path)
		goto fail;

	if (entry->server->type == GLUSTER_TRANSPORT_UNIX) {
		if (!strlen(entry->server->u.uds.socket) ||
		    !strlen(entry->volname) || !strlen(entry->path))
			goto fail;
	} else {
		if (!strlen(entry->server->u.inet.addr) ||
		    !strlen(entry->volname) || !strlen(entry->path))
			goto fail;
	}

	free(origp);

	return 0;

fail:
	gluster_free_server(hosts);
	free(origp);

	return -1;
}
