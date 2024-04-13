set_desktop_file_keys_from_client (GsmClient *client,
                                   GKeyFile  *keyfile)
{
        SmProp *prop;
        char   *name;
        char   *comment;

        prop = find_property (GSM_XSMP_CLIENT (client), SmProgram, NULL);
        name = g_strdup (prop->vals[0].value);

        comment = g_strdup_printf ("Client %s which was automatically saved",
                                   gsm_client_peek_startup_id (client));

        g_key_file_set_string (keyfile,
                               G_KEY_FILE_DESKTOP_GROUP,
                               G_KEY_FILE_DESKTOP_KEY_NAME,
                               name);

        g_key_file_set_string (keyfile,
                               G_KEY_FILE_DESKTOP_GROUP,
                               G_KEY_FILE_DESKTOP_KEY_COMMENT,
                               comment);

        g_key_file_set_string (keyfile,
                               G_KEY_FILE_DESKTOP_GROUP,
                               G_KEY_FILE_DESKTOP_KEY_ICON,
                               "system-run");

        g_key_file_set_string (keyfile,
                               G_KEY_FILE_DESKTOP_GROUP,
                               G_KEY_FILE_DESKTOP_KEY_TYPE,
                               "Application");

        g_key_file_set_boolean (keyfile,
                                G_KEY_FILE_DESKTOP_GROUP,
                                G_KEY_FILE_DESKTOP_KEY_STARTUP_NOTIFY,
                                TRUE);

        g_free (name);
        g_free (comment);
}
