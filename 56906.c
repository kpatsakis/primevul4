xsmp_cancel_end_session (GsmClient *client,
                         GError   **error)
{
        GsmXSMPClient *xsmp = (GsmXSMPClient *) client;

        g_debug ("GsmXSMPClient: xsmp_cancel_end_session ('%s')", xsmp->priv->description);

        if (xsmp->priv->conn == NULL) {
                g_set_error (error,
                             GSM_CLIENT_ERROR,
                             GSM_CLIENT_ERROR_NOT_REGISTERED,
                             "Client is not registered");
                return FALSE;
        }

        SmsShutdownCancelled (xsmp->priv->conn);

        /* reset the state */
        xsmp->priv->current_save_yourself = -1;
        xsmp->priv->next_save_yourself = -1;
        xsmp->priv->next_save_yourself_allow_interact = FALSE;

        return TRUE;
}
