free_key_string(decryption_key_t *dk)
{
    if (dk->key)
        g_string_free(dk->key, TRUE);
    if (dk->ssid)
        g_byte_array_free(dk->ssid, TRUE);
    g_free(dk);
}
