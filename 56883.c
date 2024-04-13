close_connection_callback (SmsConn     conn,
                           SmPointer   manager_data,
                           int         count,
                           char      **reason_msgs)
{
        GsmXSMPClient *client = manager_data;
        int            i;

        g_debug ("GsmXSMPClient: Client '%s' received CloseConnection", client->priv->description);
        for (i = 0; i < count; i++) {
                g_debug ("GsmXSMPClient:  close reason: '%s'", reason_msgs[i]);
        }
        SmFreeReasons (count, reason_msgs);

        gsm_client_set_status (GSM_CLIENT (client), GSM_CLIENT_FINISHED);
        gsm_client_disconnected (GSM_CLIENT (client));
}
