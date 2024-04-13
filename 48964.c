krb5_recvauth_version(krb5_context context,
                      krb5_auth_context *auth_context,
                      /* IN */
                      krb5_pointer fd,
                      krb5_principal server,
                      krb5_int32 flags,
                      krb5_keytab keytab,
                      /* OUT */
                      krb5_ticket **ticket,
                      krb5_data *version)
{
    return recvauth_common (context, auth_context, fd, 0,
                            server, flags, keytab, ticket, version);
}
