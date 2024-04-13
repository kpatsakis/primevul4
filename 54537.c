_kdc_r_log(kdc_request_t r, int level, const char *fmt, ...)
{
    va_list ap;
    char *s;
    va_start(ap, fmt);
    s = kdc_log_msg_va(r->context, r->config, level, fmt, ap);
    if(s) free(s);
    va_end(ap);
}
