ptvcursor_add_invalid_check(ptvcursor_t *csr, int hf, gint len, guint64 invalid_val) {
    proto_item *ti;
    guint64     val = invalid_val;

    switch (len) {
        case 8:
            val = tvb_get_letoh64(ptvcursor_tvbuff(csr),
                ptvcursor_current_offset(csr));
            break;
        case 4:
            val = tvb_get_letohl(ptvcursor_tvbuff(csr),
                ptvcursor_current_offset(csr));
            break;
        case 2:
            val = tvb_get_letohs(ptvcursor_tvbuff(csr),
                ptvcursor_current_offset(csr));
            break;
        case 1:
            val = tvb_get_guint8(ptvcursor_tvbuff(csr),
                ptvcursor_current_offset(csr));
            break;
        default:
            DISSECTOR_ASSERT_NOT_REACHED();
    }

    ti = ptvcursor_add(csr, hf, len, ENC_LITTLE_ENDIAN);
    if (val == invalid_val)
        proto_item_append_text(ti, " [invalid]");
}
