update_iceauthority (GsmXsmpServer *server,
                     gboolean       adding)
{
        char             *filename;
        char            **our_network_ids;
        FILE             *fp;
        IceAuthFileEntry *auth_entry;
        GSList           *entries;
        GSList           *e;
        int               i;
        gboolean          ok = FALSE;

        filename = IceAuthFileName ();
        if (IceLockAuthFile (filename,
                             GSM_ICE_AUTH_RETRIES,
                             GSM_ICE_AUTH_INTERVAL,
                             GSM_ICE_AUTH_LOCK_TIMEOUT) != IceAuthLockSuccess) {
                return FALSE;
        }

        our_network_ids = g_malloc (server->priv->num_local_xsmp_sockets * sizeof (char *));
        for (i = 0; i < server->priv->num_local_xsmp_sockets; i++) {
                our_network_ids[i] = IceGetListenConnectionString (server->priv->xsmp_sockets[i]);
        }

        entries = NULL;

        fp = fopen (filename, "r+");
        if (fp != NULL) {
                while ((auth_entry = IceReadAuthFileEntry (fp)) != NULL) {
                        /* Skip/delete entries with no network ID (invalid), or with
                         * our network ID; if we're starting up, an entry with our
                         * ID must be a stale entry left behind by an old process,
                         * and if we're shutting down, it won't be valid in the
                         * future, so either way we want to remove it from the list.
                         */
                        if (!auth_entry->network_id) {
                                IceFreeAuthFileEntry (auth_entry);
                                continue;
                        }

                        for (i = 0; i < server->priv->num_local_xsmp_sockets; i++) {
                                if (!strcmp (auth_entry->network_id, our_network_ids[i])) {
                                        IceFreeAuthFileEntry (auth_entry);
                                        break;
                                }
                        }
                        if (i != server->priv->num_local_xsmp_sockets) {
                                continue;
                        }

                        entries = g_slist_prepend (entries, auth_entry);
                }

                rewind (fp);
        } else {
                int fd;

                if (g_file_test (filename, G_FILE_TEST_EXISTS)) {
                        g_warning ("Unable to read ICE authority file: %s", filename);
                        goto cleanup;
                }

                fd = open (filename, O_CREAT | O_WRONLY, 0600);
                fp = fdopen (fd, "w");
                if (!fp) {
                        g_warning ("Unable to write to ICE authority file: %s", filename);
                        if (fd != -1) {
                                close (fd);
                        }
                        goto cleanup;
                }
        }

        if (adding) {
                for (i = 0; i < server->priv->num_local_xsmp_sockets; i++) {
                        entries = g_slist_append (entries,
                                                  auth_entry_new ("ICE", our_network_ids[i]));
                        entries = g_slist_prepend (entries,
                                                   auth_entry_new ("XSMP", our_network_ids[i]));
                }
        }

        for (e = entries; e; e = e->next) {
                IceAuthFileEntry *auth_entry = e->data;
                IceWriteAuthFileEntry (fp, auth_entry);
                IceFreeAuthFileEntry (auth_entry);
        }
        g_slist_free (entries);

        fclose (fp);
        ok = TRUE;

 cleanup:
        IceUnlockAuthFile (filename);
        for (i = 0; i < server->priv->num_local_xsmp_sockets; i++) {
                free (our_network_ids[i]);
        }
        g_free (our_network_ids);

        return ok;
}
