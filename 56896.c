gsm_xsmp_client_new (IceConn ice_conn)
{
        GsmXSMPClient *xsmp;

        xsmp = g_object_new (GSM_TYPE_XSMP_CLIENT,
                             "ice-connection", ice_conn,
                             NULL);

        return GSM_CLIENT (xsmp);
}
