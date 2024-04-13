xsmp_get_unix_process_id (GsmClient *client)
{
        SmProp  *prop;
        guint    pid;
        gboolean res;

        g_debug ("GsmXSMPClient: getting pid");

        prop = find_property (GSM_XSMP_CLIENT (client), SmProcessID, NULL);

        if (!prop || strcmp (prop->type, SmARRAY8) != 0) {
                return 0;
        }

        pid = 0;
        res = _parse_value_as_uint ((char *)prop->vals[0].value, &pid);
        if (! res) {
                pid = 0;
        }

        return pid;
}
