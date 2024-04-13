gsm_xsmp_server_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
        GsmXsmpServer *self;

        self = GSM_XSMP_SERVER (object);

        switch (prop_id) {
        case PROP_CLIENT_STORE:
                gsm_xsmp_server_set_client_store (self, g_value_get_object (value));
                break;
         default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}
