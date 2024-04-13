xsmp_stop (GsmClient *client,
           GError   **error)
{
        GsmXSMPClient *xsmp = (GsmXSMPClient *) client;

        g_debug ("GsmXSMPClient: xsmp_stop ('%s')", xsmp->priv->description);

        if (xsmp->priv->conn == NULL) {
                g_set_error (error,
                             GSM_CLIENT_ERROR,
                             GSM_CLIENT_ERROR_NOT_REGISTERED,
                             "Client is not registered");
                return FALSE;
        }

        SmsDie (xsmp->priv->conn);

        return TRUE;
}
