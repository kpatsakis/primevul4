auth_entry_new (const char *protocol,
                const char *network_id)
{
        IceAuthFileEntry *file_entry;
        IceAuthDataEntry  data_entry;

        file_entry = malloc (sizeof (IceAuthFileEntry));

        file_entry->protocol_name = strdup (protocol);
        file_entry->protocol_data = NULL;
        file_entry->protocol_data_length = 0;
        file_entry->network_id = strdup (network_id);
        file_entry->auth_name = strdup (GSM_ICE_MAGIC_COOKIE_AUTH_NAME);
        file_entry->auth_data = IceGenerateMagicCookie (GSM_ICE_MAGIC_COOKIE_LEN);
        file_entry->auth_data_length = GSM_ICE_MAGIC_COOKIE_LEN;

        /* Also create an in-memory copy, which is what the server will
         * actually use for checking client auth.
         */
        data_entry.protocol_name = file_entry->protocol_name;
        data_entry.network_id = file_entry->network_id;
        data_entry.auth_name = file_entry->auth_name;
        data_entry.auth_data = file_entry->auth_data;
        data_entry.auth_data_length = file_entry->auth_data_length;
        IceSetPaAuthData (1, &data_entry);

        return file_entry;
}
