gsm_xsmp_client_set_property (GObject       *object,
                              guint          prop_id,
                              const GValue  *value,
                              GParamSpec    *pspec)
{
        GsmXSMPClient *self;

        self = GSM_XSMP_CLIENT (object);

        switch (prop_id) {
        case PROP_ICE_CONNECTION:
                gsm_client_set_ice_connection (self, g_value_get_pointer (value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}
