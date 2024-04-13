interact_done_callback (SmsConn   conn,
                        SmPointer manager_data,
                        Bool      cancel_shutdown)
{
        GsmXSMPClient *client = manager_data;

        g_debug ("GsmXSMPClient: Client '%s' received InteractDone(cancel_shutdown = %s)",
                 client->priv->description,
                 cancel_shutdown ? "True" : "False");

        gsm_client_end_session_response (GSM_CLIENT (client),
                                         TRUE, FALSE, cancel_shutdown,
                                         NULL);
}
