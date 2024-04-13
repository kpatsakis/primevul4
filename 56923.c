gsm_xsmp_server_set_client_store (GsmXsmpServer *xsmp_server,
                                  GsmStore      *store)
{
        g_return_if_fail (GSM_IS_XSMP_SERVER (xsmp_server));

        if (store != NULL) {
                g_object_ref (store);
        }

        if (xsmp_server->priv->client_store != NULL) {
                g_object_unref (xsmp_server->priv->client_store);
        }

        xsmp_server->priv->client_store = store;
}
