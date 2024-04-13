_parse_value_as_uint (const char *value,
                      guint      *uintval)
{
        char  *end_of_valid_uint;
        gulong ulong_value;
        guint  uint_value;

        errno = 0;
        ulong_value = strtoul (value, &end_of_valid_uint, 10);

        if (*value == '\0' || *end_of_valid_uint != '\0') {
                return FALSE;
        }

        uint_value = ulong_value;
        if (uint_value != ulong_value || errno == ERANGE) {
                return FALSE;
        }

        *uintval = uint_value;

        return TRUE;
}
