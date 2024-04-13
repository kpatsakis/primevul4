save_yourself_done_callback (SmsConn   conn,
                             SmPointer manager_data,
                             Bool      success)
{
        GsmXSMPClient *client = manager_data;

        g_debug ("GsmXSMPClient: Client '%s' received SaveYourselfDone(success = %s)",
                 client->priv->description,
                 success ? "True" : "False");

	if (client->priv->current_save_yourself != -1) {
		SmsSaveComplete (client->priv->conn);
		client->priv->current_save_yourself = -1;
	}

        /* If success is false then the application couldn't save data. Nothing
         * the session manager can do about, though. FIXME: we could display a
         * dialog about this, I guess. */
        gsm_client_end_session_response (GSM_CLIENT (client),
                                         TRUE, FALSE, FALSE,
                                         NULL);

        if (client->priv->next_save_yourself) {
                int      save_type = client->priv->next_save_yourself;
                gboolean allow_interact = client->priv->next_save_yourself_allow_interact;

                client->priv->next_save_yourself = -1;
                client->priv->next_save_yourself_allow_interact = -1;
                do_save_yourself (client, save_type, allow_interact);
        }
}
