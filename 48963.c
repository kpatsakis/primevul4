krb5_recvauth(krb5_context context, krb5_auth_context *auth_context, krb5_pointer fd, char *appl_version, krb5_principal server, krb5_int32 flags, krb5_keytab keytab, krb5_ticket **ticket)
{
    return recvauth_common (context, auth_context, fd, appl_version,
                            server, flags, keytab, ticket, 0);
}
