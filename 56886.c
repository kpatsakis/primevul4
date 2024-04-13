delete_properties_callback (SmsConn     conn,
                            SmPointer   manager_data,
                            int         num_props,
                            char      **prop_names)
{
        GsmXSMPClient *client = manager_data;
        int i;

        g_debug ("GsmXSMPClient: Delete properties from '%s'", client->priv->description);

        for (i = 0; i < num_props; i++) {
                delete_property (client, prop_names[i]);

                g_debug ("  %s", prop_names[i]);
        }

        free (prop_names);
}
