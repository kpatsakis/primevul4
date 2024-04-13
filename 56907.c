xsmp_end_session (GsmClient *client,
                  guint      flags,
                  GError   **error)
{
        gboolean phase2;

        if (GSM_XSMP_CLIENT (client)->priv->conn == NULL) {
                g_set_error (error,
                             GSM_CLIENT_ERROR,
                             GSM_CLIENT_ERROR_NOT_REGISTERED,
                             "Client is not registered");
                return FALSE;
        }

        phase2 = (flags & GSM_CLIENT_END_SESSION_FLAG_LAST);

        if (phase2) {
                xsmp_save_yourself_phase2 (client);
        } else {
                gboolean allow_interact;
                int      save_type;

                /* we gave a chance to interact to the app during
                 * xsmp_query_end_session(), now it's too late to interact */
                allow_interact = FALSE;

                if (flags & GSM_CLIENT_END_SESSION_FLAG_SAVE) {
                        save_type = SmSaveBoth;
                } else {
                        save_type = SmSaveGlobal;
                }

                do_save_yourself (GSM_XSMP_CLIENT (client),
                                  save_type, allow_interact);
        }

        return TRUE;
}
