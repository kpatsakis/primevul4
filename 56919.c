gsm_xsmp_server_constructor (GType                  type,
                             guint                  n_construct_properties,
                             GObjectConstructParam *construct_properties)
{
        GsmXsmpServer *xsmp_server;

        xsmp_server = GSM_XSMP_SERVER (G_OBJECT_CLASS (gsm_xsmp_server_parent_class)->constructor (type,
                                                                                       n_construct_properties,
                                                                                       construct_properties));
        setup_listener (xsmp_server);

        return G_OBJECT (xsmp_server);
}
