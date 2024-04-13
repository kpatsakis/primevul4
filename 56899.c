prop_to_command (SmProp *prop)
{
        GString *str;
        int i, j;
        gboolean need_quotes;

        str = g_string_new (NULL);
        for (i = 0; i < prop->num_vals; i++) {
                char *val = prop->vals[i].value;

                need_quotes = FALSE;
                for (j = 0; j < prop->vals[i].length; j++) {
                        if (!g_ascii_isalnum (val[j]) && !strchr ("-_=:./", val[j])) {
                                need_quotes = TRUE;
                                break;
                        }
                }

                if (i > 0) {
                        g_string_append_c (str, ' ');
                }

                if (!need_quotes) {
                        g_string_append_printf (str,
                                                "%.*s",
                                                prop->vals[i].length,
                                                (char *)prop->vals[i].value);
                } else {
                        g_string_append_c (str, '\'');
                        while (val < (char *)prop->vals[i].value + prop->vals[i].length) {
                                if (*val == '\'') {
                                        g_string_append (str, "'\''");
                                } else {
                                        g_string_append_c (str, *val);
                                }
                                val++;
                        }
                        g_string_append_c (str, '\'');
                }
        }

        return g_string_free (str, FALSE);
}
