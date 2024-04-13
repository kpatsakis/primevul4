delete_property (GsmXSMPClient *client,
                 const char    *name)
{
        int     index;
        SmProp *prop;

        prop = find_property (client, name, &index);
        if (!prop) {
                return;
        }

#if 0
        /* This is wrong anyway; we can't unconditionally run the current
         * discard command; if this client corresponds to a GsmAppResumed,
         * and the current discard command is identical to the app's
         * discard_command, then we don't run the discard command now,
         * because that would delete a saved state we may want to resume
         * again later.
         */
        if (!strcmp (name, SmDiscardCommand)) {
                gsm_client_run_discard (GSM_CLIENT (client));
        }
#endif

        g_ptr_array_remove_index_fast (client->priv->props, index);
        SmFreeProperty (prop);
}
