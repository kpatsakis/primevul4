xsmp_save_yourself_phase2 (GsmClient *client)
{
        GsmXSMPClient *xsmp = (GsmXSMPClient *) client;

        g_debug ("GsmXSMPClient: xsmp_save_yourself_phase2 ('%s')", xsmp->priv->description);

        SmsSaveYourselfPhase2 (xsmp->priv->conn);
}
