gsm_xsmp_client_init (GsmXSMPClient *client)
{
        client->priv = GSM_XSMP_CLIENT_GET_PRIVATE (client);

        client->priv->props = g_ptr_array_new ();
        client->priv->current_save_yourself = -1;
        client->priv->next_save_yourself = -1;
        client->priv->next_save_yourself_allow_interact = FALSE;
}
