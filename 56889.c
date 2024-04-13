 find_property (GsmXSMPClient *client,
                const char    *name,
               int           *index)
{
        SmProp *prop;
        int i;

        for (i = 0; i < client->priv->props->len; i++) {
                prop = client->priv->props->pdata[i];

                if (!strcmp (prop->name, name)) {
                        if (index) {
                                *index = i;
                        }
                        return prop;
                }
        }

        return NULL;
}
