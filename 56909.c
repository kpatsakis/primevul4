xsmp_get_restart_command (GsmClient *client)
{
        SmProp *prop;

        prop = find_property (GSM_XSMP_CLIENT (client), SmRestartCommand, NULL);

        if (!prop || strcmp (prop->type, SmLISTofARRAY8) != 0) {
                return NULL;
        }

        return prop_to_command (prop);
}
