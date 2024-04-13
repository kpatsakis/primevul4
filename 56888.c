do_save_yourself (GsmXSMPClient *client,
                  int            save_type,
                  gboolean       allow_interact)
{
        g_assert (client->priv->conn != NULL);

        if (client->priv->next_save_yourself != -1) {
                /* Either we're currently doing a shutdown and there's a checkpoint
                 * queued after it, or vice versa. Either way, the new SaveYourself
                 * is redundant.
                 */
                g_debug ("GsmXSMPClient:   skipping redundant SaveYourself for '%s'",
                         client->priv->description);
        } else if (client->priv->current_save_yourself != -1) {
                g_debug ("GsmXSMPClient:   queuing new SaveYourself for '%s'",
                         client->priv->description);
                client->priv->next_save_yourself = save_type;
                client->priv->next_save_yourself_allow_interact = allow_interact;
        } else {
                client->priv->current_save_yourself = save_type;
                /* make sure we don't have anything queued */
                client->priv->next_save_yourself = -1;
                client->priv->next_save_yourself_allow_interact = FALSE;

                switch (save_type) {
                case SmSaveLocal:
                        /* Save state */
                        SmsSaveYourself (client->priv->conn,
                                         SmSaveLocal,
                                         FALSE,
                                         SmInteractStyleNone,
                                         FALSE);
                        break;

                default:
                        /* Logout */
                        if (!allow_interact) {
                                SmsSaveYourself (client->priv->conn,
                                                 save_type, /* save type */
                                                 TRUE, /* shutdown */
                                                 SmInteractStyleNone, /* interact style */
                                                 TRUE); /* fast */
                        } else {
                                SmsSaveYourself (client->priv->conn,
                                                 save_type, /* save type */
                                                 TRUE, /* shutdown */
                                                 SmInteractStyleAny, /* interact style */
                                                 FALSE /* fast */);
                        }
                        break;
                }
        }
}
