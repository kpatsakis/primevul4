gsm_xsmp_server_class_init (GsmXsmpServerClass *klass)
{
        GObjectClass   *object_class = G_OBJECT_CLASS (klass);

        object_class->get_property = gsm_xsmp_server_get_property;
        object_class->set_property = gsm_xsmp_server_set_property;
        object_class->constructor = gsm_xsmp_server_constructor;
        object_class->finalize = gsm_xsmp_server_finalize;

        g_object_class_install_property (object_class,
                                         PROP_CLIENT_STORE,
                                         g_param_spec_object ("client-store",
                                                              NULL,
                                                              NULL,
                                                              GSM_TYPE_STORE,
                                                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

        g_type_class_add_private (klass, sizeof (GsmXsmpServerPrivate));
}
