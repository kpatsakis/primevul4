parse_key_string(gchar* input_string, guint8 key_type)
{
    gchar *key, *tmp_str;
    gchar *ssid;

    GString    *key_string = NULL;
    GByteArray *ssid_ba = NULL, *key_ba;
    gboolean    res;

    gchar **tokens;
    guint n = 0;
    decryption_key_t *dk;

    if(input_string == NULL)
        return NULL;

    /*
     * Parse the input_string. WEP and WPA will be just a string
     * of hexadecimal characters (if key is wrong, null will be
     * returned...).
     * WPA-PWD should be in the form
     * <key data>[:<ssid>]
     */

    switch(key_type)
    {
    case AIRPDCAP_KEY_TYPE_WEP:
    case AIRPDCAP_KEY_TYPE_WEP_40:
    case AIRPDCAP_KEY_TYPE_WEP_104:

       key_ba = g_byte_array_new();
       res = hex_str_to_bytes(input_string, key_ba, FALSE);

       if (res && key_ba->len > 0) {
           /* Key is correct! It was probably an 'old style' WEP key */
           /* Create the decryption_key_t structure, fill it and return it*/
           dk = (decryption_key_t *)g_malloc(sizeof(decryption_key_t));

           dk->type = AIRPDCAP_KEY_TYPE_WEP;
           /* XXX - The current key handling code in the GUI requires
            * no separators and lower case */
           tmp_str = bytes_to_str(NULL, key_ba->data, key_ba->len);
           dk->key  = g_string_new(tmp_str);
           g_string_ascii_down(dk->key);
           dk->bits = key_ba->len * 8;
           dk->ssid = NULL;

           wmem_free(NULL, tmp_str);
           g_byte_array_free(key_ba, TRUE);
           return dk;
       }

       /* Key doesn't work */
       g_byte_array_free(key_ba, TRUE);
       return NULL;

    case AIRPDCAP_KEY_TYPE_WPA_PWD:

        tokens = g_strsplit(input_string,":",0);

        /* Tokens is a null termiated array of strings ... */
        while(tokens[n] != NULL)
            n++;

        if(n < 1)
        {
            /* Free the array of strings */
            g_strfreev(tokens);
            return NULL;
        }

        /*
         * The first token is the key
         */
        key = g_strdup(tokens[0]);

        ssid = NULL;
        /* Maybe there is a second token (an ssid, if everything else is ok) */
        if(n >= 2)
        {
           ssid = g_strdup(tokens[1]);
        }

        /* Create a new string */
        key_string = g_string_new(key);
        ssid_ba = NULL;

        /* Two (or more) tokens mean that the user entered a WPA-PWD key ... */
        if( ((key_string->len) > WPA_KEY_MAX_CHAR_SIZE) || ((key_string->len) < WPA_KEY_MIN_CHAR_SIZE))
        {
            g_string_free(key_string, TRUE);

            g_free(key);
            g_free(ssid);

            /* Free the array of strings */
            g_strfreev(tokens);
            return NULL;
        }

        if(ssid != NULL) /* more than two tokens found, means that the user specified the ssid */
        {
            ssid_ba = g_byte_array_new();
            if (! uri_str_to_bytes(ssid, ssid_ba)) {
                g_string_free(key_string, TRUE);
                g_byte_array_free(ssid_ba, TRUE);
                g_free(key);
                g_free(ssid);
                /* Free the array of strings */
                g_strfreev(tokens);
                return NULL;
            }

            if(ssid_ba->len > WPA_SSID_MAX_CHAR_SIZE)
            {
                g_string_free(key_string, TRUE);
                g_byte_array_free(ssid_ba, TRUE);

                g_free(key);
                g_free(ssid);

                /* Free the array of strings */
                g_strfreev(tokens);
                return NULL;
            }
        }

        /* Key was correct!!! Create the new decryption_key_t ... */
        dk = (decryption_key_t*)g_malloc(sizeof(decryption_key_t));

        dk->type = AIRPDCAP_KEY_TYPE_WPA_PWD;
        dk->key  = g_string_new(key);
        dk->bits = 256; /* This is the length of the array pf bytes that will be generated using key+ssid ...*/
        dk->ssid = byte_array_dup(ssid_ba); /* NULL if ssid_ba is NULL */

        g_string_free(key_string, TRUE);
        if (ssid_ba != NULL)
            g_byte_array_free(ssid_ba, TRUE);

        g_free(key);
        if(ssid != NULL)
            g_free(ssid);

        /* Free the array of strings */
        g_strfreev(tokens);
        return dk;

    case AIRPDCAP_KEY_TYPE_WPA_PSK:

        key_ba = g_byte_array_new();
        res = hex_str_to_bytes(input_string, key_ba, FALSE);

        /* Two tokens means that the user should have entered a WPA-BIN key ... */
        if(!res || ((key_ba->len) != WPA_PSK_KEY_SIZE))
        {
            g_byte_array_free(key_ba, TRUE);

            /* No ssid has been created ... */
            return NULL;
        }

        /* Key was correct!!! Create the new decryption_key_t ... */
        dk = (decryption_key_t*)g_malloc(sizeof(decryption_key_t));

        dk->type = AIRPDCAP_KEY_TYPE_WPA_PSK;
        dk->key  = g_string_new(input_string);
        dk->bits = (guint) dk->key->len * 4;
        dk->ssid = NULL;

        g_byte_array_free(key_ba, TRUE);
        return dk;
    }

    /* Type not supported */
    return NULL;
}
