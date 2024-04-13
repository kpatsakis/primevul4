void setupexport(CLIENT* client) {
	int i;
	off_t laststartoff = 0, lastsize = 0;
	int multifile = (client->server->flags & F_MULTIFILE);
	int temporary = (client->server->flags & F_TEMPORARY) && !multifile;
	int cancreate = (client->server->expected_size) && !multifile;

	client->export = g_array_new(TRUE, TRUE, sizeof(FILE_INFO));

	/* If multi-file, open as many files as we can.
	 * If not, open exactly one file.
	 * Calculate file sizes as we go to get total size. */
	for(i=0; ; i++) {
		FILE_INFO fi;
		gchar *tmpname;
		gchar* error_string;

		if (i)
		  cancreate = 0;
		/* if expected_size is specified, and this is the first file, we can create the file */
		mode_t mode = (client->server->flags & F_READONLY) ?
		  O_RDONLY : (O_RDWR | (cancreate?O_CREAT:0));

		if (temporary) {
			tmpname=g_strdup_printf("%s.%d-XXXXXX", client->exportname, i);
			DEBUG( "Opening %s\n", tmpname );
			fi.fhandle = mkstemp(tmpname);
		} else {
			if(multifile) {
				tmpname=g_strdup_printf("%s.%d", client->exportname, i);
			} else {
				tmpname=g_strdup(client->exportname);
			}
			DEBUG( "Opening %s\n", tmpname );
			fi.fhandle = open(tmpname, mode, 0x600);
			if(fi.fhandle == -1 && mode == O_RDWR) {
				/* Try again because maybe media was read-only */
				fi.fhandle = open(tmpname, O_RDONLY);
				if(fi.fhandle != -1) {
					/* Opening the base file in copyonwrite mode is
					 * okay */
					if(!(client->server->flags & F_COPYONWRITE)) {
						client->server->flags |= F_AUTOREADONLY;
						client->server->flags |= F_READONLY;
					}
				}
			}
		}
		if(fi.fhandle == -1) {
			if(multifile && i>0)
				break;
			error_string=g_strdup_printf(
				"Could not open exported file %s: %%m",
				tmpname);
			err(error_string);
		}

		if (temporary)
			unlink(tmpname); /* File will stick around whilst FD open */

		fi.startoff = laststartoff + lastsize;
		g_array_append_val(client->export, fi);
		g_free(tmpname);

		/* Starting offset and size of this file will be used to
		 * calculate starting offset of next file */
		laststartoff = fi.startoff;
		lastsize = size_autodetect(fi.fhandle);

		/* If we created the file, it will be length zero */
		if (!lastsize && cancreate) {
			assert(!multifile);
			if(ftruncate (fi.fhandle, client->server->expected_size)<0) {
				err("Could not expand file: %m");
			}
			lastsize = client->server->expected_size;
			break; /* don't look for any more files */
		}

		if(!multifile || temporary)
			break;
	}

	/* Set export size to total calculated size */
	client->exportsize = laststartoff + lastsize;

	/* Export size may be overridden */
	if(client->server->expected_size) {
		/* desired size must be <= total calculated size */
		if(client->server->expected_size > client->exportsize) {
			err("Size of exported file is too big\n");
		}

		client->exportsize = client->server->expected_size;
	}

	msg(LOG_INFO, "Size of exported file/device is %llu", (unsigned long long)client->exportsize);
	if(multifile) {
		msg(LOG_INFO, "Total number of files: %d", i);
	}
}
