gsm_xsmp_client_constructor (GType                  type,
                             guint                  n_construct_properties,
                             GObjectConstructParam *construct_properties)
{
        GsmXSMPClient *client;

        client = GSM_XSMP_CLIENT (G_OBJECT_CLASS (gsm_xsmp_client_parent_class)->constructor (type,
                                                                                              n_construct_properties,
                                                                                              construct_properties));
        setup_connection (client);

        return G_OBJECT (client);
}
