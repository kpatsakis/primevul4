gsm_xsmp_server_start (GsmXsmpServer *server)
{
        GIOChannel *channel;
        int         i;

        for (i = 0; i < server->priv->num_local_xsmp_sockets; i++) {
                GsmIceConnectionData *data;

                data = g_new0 (GsmIceConnectionData, 1);
                data->server = server;
                data->listener = server->priv->xsmp_sockets[i];

                channel = g_io_channel_unix_new (IceGetListenConnectionNumber (server->priv->xsmp_sockets[i]));
                g_io_add_watch_full (channel,
                                     G_PRIORITY_DEFAULT,
                                     G_IO_IN | G_IO_HUP | G_IO_ERR,
                                     (GIOFunc)accept_ice_connection,
                                     data,
                                     (GDestroyNotify)g_free);
                g_io_channel_unref (channel);
        }
}
