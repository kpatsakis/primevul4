int expflush(CLIENT *client) {
	gint i;

        if (client->server->flags & F_COPYONWRITE) {
		return fsync(client->difffile);
	}
	
	for (i = 0; i < client->export->len; i++) {
		FILE_INFO fi = g_array_index(client->export, FILE_INFO, i);
		if (fsync(fi.fhandle) < 0)
			return -1;
	}
	
	return 0;
}
