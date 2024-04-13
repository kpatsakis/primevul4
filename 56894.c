gsm_xsmp_client_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
        GsmXSMPClient *self;

        self = GSM_XSMP_CLIENT (object);

        switch (prop_id) {
        case PROP_ICE_CONNECTION:
                g_value_set_pointer (value, self->priv->ice_connection);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}
