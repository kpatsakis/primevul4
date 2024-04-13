crm_xml_err(void *ctx, const char *msg, ...)
{
    int len = 0;
    va_list args;
    char *buf = NULL;
    static int buffer_len = 0;
    static char *buffer = NULL;
    static struct qb_log_callsite *xml_error_cs = NULL;

    va_start(args, msg);
    len = vasprintf(&buf, msg, args);

    if(xml_error_cs == NULL) {
        xml_error_cs = qb_log_callsite_get(
            __func__, __FILE__, "xml library error", LOG_TRACE, __LINE__, crm_trace_nonlog);
    }

    if (strchr(buf, '\n')) {
        buf[len - 1] = 0;
        if (buffer) {
            crm_err("XML Error: %s%s", buffer, buf);
            free(buffer);
        } else {
            crm_err("XML Error: %s", buf);
        }
        if (xml_error_cs && xml_error_cs->targets) {
            crm_abort(__FILE__, __PRETTY_FUNCTION__, __LINE__, "xml library error", TRUE, TRUE);
        }
        buffer = NULL;
        buffer_len = 0;

    } else if (buffer == NULL) {
        buffer_len = len;
        buffer = buf;
        buf = NULL;

    } else {
        buffer = realloc_safe(buffer, 1 + buffer_len + len);
        memcpy(buffer + buffer_len, buf, len);
        buffer_len += len;
        buffer[buffer_len] = 0;
    }

    va_end(args);
    free(buf);
}
