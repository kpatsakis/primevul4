xsmp_save (GsmClient *client,
           GError   **error)
{
        GsmClientRestartStyle restart_style;

        GKeyFile *keyfile = NULL;
        char     *desktop_file_path = NULL;
        char     *exec_program = NULL;
        char     *exec_discard = NULL;
        char     *startup_id = NULL;
        GError   *local_error;

        g_debug ("GsmXSMPClient: saving client with id %s",
                 gsm_client_peek_id (client));

        local_error = NULL;

        restart_style = xsmp_get_restart_style_hint (client);
        if (restart_style == GSM_CLIENT_RESTART_NEVER) {
                goto out;
        }

        exec_program = xsmp_get_restart_command (client);
        if (!exec_program) {
                goto out;
        }

        desktop_file_path = get_desktop_file_path (GSM_XSMP_CLIENT (client));

        keyfile = create_client_key_file (client,
                                          desktop_file_path,
                                          &local_error);

        if (local_error) {
                goto out;
        }

        g_object_get (client,
                      "startup-id", &startup_id,
                      NULL);

        g_key_file_set_string (keyfile,
                               G_KEY_FILE_DESKTOP_GROUP,
                               GSM_AUTOSTART_APP_STARTUP_ID_KEY,
                               startup_id);

        g_key_file_set_string (keyfile,
                               G_KEY_FILE_DESKTOP_GROUP,
                               G_KEY_FILE_DESKTOP_KEY_EXEC,
                               exec_program);

        exec_discard = xsmp_get_discard_command (client);
        if (exec_discard)
                g_key_file_set_string (keyfile,
                                       G_KEY_FILE_DESKTOP_GROUP,
                                       GSM_AUTOSTART_APP_DISCARD_KEY,
                                       exec_discard);

out:
        g_free (desktop_file_path);
        g_free (exec_program);
        g_free (exec_discard);
        g_free (startup_id);

        if (local_error != NULL) {
                g_propagate_error (error, local_error);
                g_key_file_free (keyfile);

                return NULL;
        }

        return keyfile;
}
