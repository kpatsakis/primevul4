gsm_xsmp_client_class_init (GsmXSMPClientClass *klass)
{
        GObjectClass   *object_class = G_OBJECT_CLASS (klass);
        GsmClientClass *client_class = GSM_CLIENT_CLASS (klass);

        object_class->finalize             = gsm_xsmp_client_finalize;
        object_class->constructor          = gsm_xsmp_client_constructor;
        object_class->get_property         = gsm_xsmp_client_get_property;
        object_class->set_property         = gsm_xsmp_client_set_property;

        client_class->impl_save                   = xsmp_save;
        client_class->impl_stop                   = xsmp_stop;
        client_class->impl_query_end_session      = xsmp_query_end_session;
        client_class->impl_end_session            = xsmp_end_session;
        client_class->impl_cancel_end_session     = xsmp_cancel_end_session;
        client_class->impl_get_app_name           = xsmp_get_app_name;
        client_class->impl_get_restart_style_hint = xsmp_get_restart_style_hint;
        client_class->impl_get_unix_process_id    = xsmp_get_unix_process_id;

        signals[REGISTER_REQUEST] =
                g_signal_new ("register-request",
                              G_OBJECT_CLASS_TYPE (object_class),
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET (GsmXSMPClientClass, register_request),
                              _boolean_handled_accumulator,
                              NULL,
                              gsm_marshal_BOOLEAN__POINTER,
                              G_TYPE_BOOLEAN,
                              1, G_TYPE_POINTER);
        signals[LOGOUT_REQUEST] =
                g_signal_new ("logout-request",
                              G_OBJECT_CLASS_TYPE (object_class),
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET (GsmXSMPClientClass, logout_request),
                              NULL,
                              NULL,
                              g_cclosure_marshal_VOID__BOOLEAN,
                              G_TYPE_NONE,
                              1, G_TYPE_BOOLEAN);

        g_object_class_install_property (object_class,
                                         PROP_ICE_CONNECTION,
                                         g_param_spec_pointer ("ice-connection",
                                                               "ice-connection",
                                                               "ice-connection",
                                                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

        g_type_class_add_private (klass, sizeof (GsmXSMPClientPrivate));
}
