get_key_string(decryption_key_t* dk)
{
    gchar* output_string = NULL;

    if(dk == NULL || dk->key == NULL)
        return NULL;

    switch(dk->type) {
        case AIRPDCAP_KEY_TYPE_WEP:
            output_string = g_strdup(dk->key->str);
            break;
        case AIRPDCAP_KEY_TYPE_WPA_PWD:
            if(dk->ssid == NULL)
                output_string = g_strdup(dk->key->str);
            else
                output_string = g_strdup_printf("%s:%s",
                    dk->key->str, format_uri(dk->ssid, ":"));
            break;
        case AIRPDCAP_KEY_TYPE_WPA_PMK:
            output_string = g_strdup(dk->key->str);
            break;
        default:
            return NULL;
    }

    return output_string;
}
