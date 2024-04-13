set_properties_callback (SmsConn     conn,
                         SmPointer   manager_data,
                         int         num_props,
                         SmProp    **props)
{
        GsmXSMPClient *client = manager_data;
        int            i;

        g_debug ("GsmXSMPClient: Set properties from client '%s'", client->priv->description);

        for (i = 0; i < num_props; i++) {
                delete_property (client, props[i]->name);
                g_ptr_array_add (client->priv->props, props[i]);

                debug_print_property (props[i]);

                if (!strcmp (props[i]->name, SmProgram))
                        set_description (client);
        }

        free (props);

}
