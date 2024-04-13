create_client_key_file (GsmClient   *client,
                        const char  *desktop_file_path,
                        GError     **error) {
        GKeyFile *keyfile;

        keyfile = g_key_file_new ();

        if (desktop_file_path != NULL) {
                g_key_file_load_from_file (keyfile,
                                           desktop_file_path,
                                           G_KEY_FILE_KEEP_COMMENTS |
                                           G_KEY_FILE_KEEP_TRANSLATIONS,
                                           error);
        } else {
                set_desktop_file_keys_from_client (client, keyfile);
        }

        return keyfile;
}
