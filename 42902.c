dispatch(void *handle, struct sockaddr *local_saddr,
         const krb5_fulladdr *remote_faddr, krb5_data *request, int is_tcp,
         verto_ctx *vctx, loop_respond_fn respond, void *arg)
{
    krb5_error_code ret;
    krb5_keytab kt = NULL;
    kadm5_server_handle_t server_handle = (kadm5_server_handle_t)handle;
    krb5_fulladdr local_faddr;
    krb5_address **local_kaddrs = NULL, local_kaddr_buf;
    krb5_data *response = NULL;

    if (local_saddr == NULL) {
        ret = krb5_os_localaddr(server_handle->context, &local_kaddrs);
        if (ret != 0)
            goto egress;

        local_faddr.address = local_kaddrs[0];
        local_faddr.port = 0;
    } else {
        local_faddr.address = &local_kaddr_buf;
        init_addr(&local_faddr, local_saddr);
    }

    ret = krb5_kt_resolve(server_handle->context, "KDB:", &kt);
    if (ret != 0) {
        krb5_klog_syslog(LOG_ERR, _("chpw: Couldn't open admin keytab %s"),
                         krb5_get_error_message(server_handle->context, ret));
        goto egress;
    }

    response = k5alloc(sizeof(krb5_data), &ret);
    if (response == NULL)
        goto egress;

    ret = process_chpw_request(server_handle->context,
                               handle,
                               server_handle->params.realm,
                               kt,
                               &local_faddr,
                               remote_faddr,
                               request,
                               response);
egress:
    if (ret)
        krb5_free_data(server_handle->context, response);
    krb5_free_addresses(server_handle->context, local_kaddrs);
    krb5_kt_close(server_handle->context, kt);
    (*respond)(arg, ret, ret == 0 ? response : NULL);
}
