_kdc_set_e_text(kdc_request_t r, const char *e_text)
{
    r->e_text = e_text;
    kdc_log(r->context, r->config, 0, "%s", e_text);
}
