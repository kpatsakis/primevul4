gsm_xsmp_server_new (GsmStore *client_store)
{
        if (xsmp_server_object != NULL) {
                g_object_ref (xsmp_server_object);
        } else {
                xsmp_server_object = g_object_new (GSM_TYPE_XSMP_SERVER,
                                                   "client-store", client_store,
                                                   NULL);

                g_object_add_weak_pointer (xsmp_server_object,
                                           (gpointer *) &xsmp_server_object);
        }

        return GSM_XSMP_SERVER (xsmp_server_object);
}
