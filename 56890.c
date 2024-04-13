get_desktop_file_path (GsmXSMPClient *client)
{
        SmProp     *prop;
        char       *desktop_file_path = NULL;
        char      **dirs;
        const char *program_name;

        /* XSMP clients using eggsmclient defines a special property
         * pointing to their respective desktop entry file */
        prop = find_property (client, GsmDesktopFile, NULL);

        if (prop) {
                GFile *file = g_file_new_for_uri (prop->vals[0].value);
                desktop_file_path = g_file_get_path (file);
                g_object_unref (file);
                goto out;
        }

        /* If we can't get desktop file from GsmDesktopFile then we
         * try to find the desktop file from its program name */
        prop = find_property (client, SmProgram, NULL);
        program_name = prop->vals[0].value;

        dirs = gsm_util_get_autostart_dirs ();

        desktop_file_path =
                gsm_util_find_desktop_file_for_app_name (program_name,
                                                         dirs);

        g_strfreev (dirs);

out:
        g_debug ("GsmXSMPClient: desktop file for client %s is %s",
                 gsm_client_peek_id (GSM_CLIENT (client)),
                 desktop_file_path ? desktop_file_path : "(null)");

        return desktop_file_path;
}
