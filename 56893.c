gsm_xsmp_client_finalize (GObject *object)
{
        GsmXSMPClient *client = (GsmXSMPClient *) object;

        g_debug ("GsmXSMPClient: xsmp_finalize (%s)", client->priv->description);
        gsm_xsmp_client_disconnect (client);

        g_free (client->priv->description);
        g_ptr_array_foreach (client->priv->props, (GFunc)SmFreeProperty, NULL);
        g_ptr_array_free (client->priv->props, TRUE);

        G_OBJECT_CLASS (gsm_xsmp_client_parent_class)->finalize (object);
}
