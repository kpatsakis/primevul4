client_iochannel_watch (GIOChannel    *channel,
                        GIOCondition   condition,
                        GsmXSMPClient *client)
{
        gboolean keep_going;

        g_object_ref (client);
        switch (IceProcessMessages (client->priv->ice_connection, NULL, NULL)) {
        case IceProcessMessagesSuccess:
                keep_going = TRUE;
                break;

        case IceProcessMessagesIOError:
                g_debug ("GsmXSMPClient: IceProcessMessagesIOError on '%s'", client->priv->description);
                gsm_client_set_status (GSM_CLIENT (client), GSM_CLIENT_FAILED);
                /* Emitting "disconnected" will eventually cause
                 * IceCloseConnection() to be called.
                 */
                gsm_client_disconnected (GSM_CLIENT (client));
                keep_going = FALSE;
                break;

        case IceProcessMessagesConnectionClosed:
                g_debug ("GsmXSMPClient: IceProcessMessagesConnectionClosed on '%s'",
                         client->priv->description);
                client->priv->ice_connection = NULL;
                keep_going = FALSE;
                break;

        default:
                g_assert_not_reached ();
        }
        g_object_unref (client);

         return keep_going;
 }
