gsm_xsmp_server_finalize (GObject *object)
{
        GsmXsmpServer *xsmp_server;

        g_return_if_fail (object != NULL);
        g_return_if_fail (GSM_IS_XSMP_SERVER (object));

        xsmp_server = GSM_XSMP_SERVER (object);

        g_return_if_fail (xsmp_server->priv != NULL);

        IceFreeListenObjs (xsmp_server->priv->num_xsmp_sockets, 
                           xsmp_server->priv->xsmp_sockets);

        if (xsmp_server->priv->client_store != NULL) {
                g_object_unref (xsmp_server->priv->client_store);
        }

        G_OBJECT_CLASS (gsm_xsmp_server_parent_class)->finalize (object);
}
