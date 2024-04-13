gsm_xsmp_server_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
        GsmXsmpServer *self;

        self = GSM_XSMP_SERVER (object);

        switch (prop_id) {
        case PROP_CLIENT_STORE:
                g_value_set_object (value, self->priv->client_store);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}
