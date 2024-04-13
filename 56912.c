xsmp_interact (GsmClient *client)
{
        GsmXSMPClient *xsmp = (GsmXSMPClient *) client;

        g_debug ("GsmXSMPClient: xsmp_interact ('%s')", xsmp->priv->description);

        SmsInteract (xsmp->priv->conn);
}
