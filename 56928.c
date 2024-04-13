setup_listener (GsmXsmpServer *server)
{
        char   error[256];
        mode_t saved_umask;
        char  *network_id_list;
        int    i;
        int    res;

        /* Set up sane error handlers */
        IceSetErrorHandler (ice_error_handler);
        IceSetIOErrorHandler (ice_io_error_handler);
        SmsSetErrorHandler (sms_error_handler);

        /* Initialize libSM; we pass NULL for hostBasedAuthProc to disable
         * host-based authentication.
         */
        res = SmsInitialize (PACKAGE,
                             VERSION,
                             (SmsNewClientProc)accept_xsmp_connection,
                             server,
                             NULL,
                             sizeof (error),
                             error);
        if (! res) {
                gsm_util_init_error (TRUE, "Could not initialize libSM: %s", error);
        }

#ifdef HAVE_X11_XTRANS_XTRANS_H
        /* By default, IceListenForConnections will open one socket for each
         * transport type known to X. We don't want connections from remote
         * hosts, so for security reasons it would be best if ICE didn't
         * even open any non-local sockets. So we use an internal ICElib
         * method to disable them here. Unfortunately, there is no way to
         * ask X what transport types it knows about, so we're forced to
         * guess.
         */
        _IceTransNoListen ("tcp");
#endif

        /* Create the XSMP socket. Older versions of IceListenForConnections
         * have a bug which causes the umask to be set to 0 on certain types
         * of failures. Probably not an issue on any modern systems, but
         * we'll play it safe.
         */
        saved_umask = umask (0);
        umask (saved_umask);
        res = IceListenForConnections (&server->priv->num_xsmp_sockets,
                                       &server->priv->xsmp_sockets,
                                       sizeof (error),
                                       error);
        if (! res) {
                gsm_util_init_error (TRUE, _("Could not create ICE listening socket: %s"), error);
        }

        umask (saved_umask);

        /* Find the local sockets in the returned socket list and move them
         * to the start of the list.
         */
        for (i = server->priv->num_local_xsmp_sockets = 0; i < server->priv->num_xsmp_sockets; i++) {
                char *id = IceGetListenConnectionString (server->priv->xsmp_sockets[i]);

                if (!strncmp (id, "local/", sizeof ("local/") - 1) ||
                    !strncmp (id, "unix/", sizeof ("unix/") - 1)) {
                        if (i > server->priv->num_local_xsmp_sockets) {
                                IceListenObj tmp;
                                tmp = server->priv->xsmp_sockets[i];
                                server->priv->xsmp_sockets[i] = server->priv->xsmp_sockets[server->priv->num_local_xsmp_sockets];
                                server->priv->xsmp_sockets[server->priv->num_local_xsmp_sockets] = tmp;
                        }
                        server->priv->num_local_xsmp_sockets++;
                }
                free (id);
        }

        if (server->priv->num_local_xsmp_sockets == 0) {
                gsm_util_init_error (TRUE, "IceListenForConnections did not return a local listener!");
        }

#ifdef HAVE_X11_XTRANS_XTRANS_H
        if (server->priv->num_local_xsmp_sockets != server->priv->num_xsmp_sockets) {
                /* Xtrans was apparently compiled with support for some
                 * non-local transport besides TCP (which we disabled above); we
                 * won't create IO watches on those extra sockets, so
                 * connections to them will never be noticed, but they're still
                 * there, which is inelegant.
                 *
                 * If the g_warning below is triggering for you and you want to
                 * stop it, the fix is to add additional _IceTransNoListen()
                 * calls above.
                 */
                network_id_list = IceComposeNetworkIdList (server->priv->num_xsmp_sockets - server->priv->num_local_xsmp_sockets,
                                                           server->priv->xsmp_sockets + server->priv->num_local_xsmp_sockets);
                g_warning ("IceListenForConnections returned %d non-local listeners: %s",
                           server->priv->num_xsmp_sockets - server->priv->num_local_xsmp_sockets,
                           network_id_list);
                free (network_id_list);
        }
#endif

        /* Update .ICEauthority with new auth entries for our socket */
        if (!update_iceauthority (server, TRUE)) {
                /* FIXME: is this really fatal? Hm... */
                gsm_util_init_error (TRUE,
                                     "Could not update ICEauthority file %s",
                                     IceAuthFileName ());
        }

        network_id_list = IceComposeNetworkIdList (server->priv->num_local_xsmp_sockets,
                                                   server->priv->xsmp_sockets);

        gsm_util_setenv ("SESSION_MANAGER", network_id_list);
        g_debug ("GsmXsmpServer: SESSION_MANAGER=%s\n", network_id_list);
        free (network_id_list);
}
