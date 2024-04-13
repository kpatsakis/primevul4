save_yourself_phase2_request_callback (SmsConn   conn,
                                       SmPointer manager_data)
{
        GsmXSMPClient *client = manager_data;

        g_debug ("GsmXSMPClient: Client '%s' received SaveYourselfPhase2Request",
                 client->priv->description);

        client->priv->current_save_yourself = -1;

        /* this is a valid response to SaveYourself and therefore
           may be a response to a QES or ES */
        gsm_client_end_session_response (GSM_CLIENT (client),
                                         TRUE, TRUE, FALSE,
                                         NULL);
}
