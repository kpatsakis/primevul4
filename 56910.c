xsmp_get_restart_style_hint (GsmClient *client)
{
        SmProp               *prop;
        GsmClientRestartStyle hint;

        g_debug ("GsmXSMPClient: getting restart style");
        hint = GSM_CLIENT_RESTART_IF_RUNNING;

        prop = find_property (GSM_XSMP_CLIENT (client), SmRestartStyleHint, NULL);

        if (!prop || strcmp (prop->type, SmCARD8) != 0) {
                return GSM_CLIENT_RESTART_IF_RUNNING;
        }

        switch (((unsigned char *)prop->vals[0].value)[0]) {
        case SmRestartIfRunning:
                hint = GSM_CLIENT_RESTART_IF_RUNNING;
                break;
        case SmRestartAnyway:
                hint = GSM_CLIENT_RESTART_ANYWAY;
                break;
        case SmRestartImmediately:
                hint = GSM_CLIENT_RESTART_IMMEDIATELY;
                break;
        case SmRestartNever:
                hint = GSM_CLIENT_RESTART_NEVER;
                break;
        default:
                break;
        }

        return hint;
}
