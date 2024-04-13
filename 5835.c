static inline const gchar *format_param_str(tvbuff_t *tvb, int offset, int len) {
    char *param_str;

    param_str = tvb_get_string_enc(wmem_packet_scope(), tvb, offset, len, ENC_UTF_8|ENC_NA);

    if (len < 2) {
        return param_str;
    }
    return format_text_chr(wmem_packet_scope(), param_str, len - 1, ' '); /* Leave terminating NULLs alone. */
}