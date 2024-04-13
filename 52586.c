comp_cksum(krb5_context kcontext, krb5_data *source, krb5_ticket *ticket,
           krb5_checksum *his_cksum)
{
    krb5_error_code       retval;
    krb5_boolean          valid;

    if (!krb5_c_valid_cksumtype(his_cksum->checksum_type))
        return KRB5KDC_ERR_SUMTYPE_NOSUPP;

    /* must be collision proof */
    if (!krb5_c_is_coll_proof_cksum(his_cksum->checksum_type))
        return KRB5KRB_AP_ERR_INAPP_CKSUM;

    /* verify checksum */
    if ((retval = krb5_c_verify_checksum(kcontext, ticket->enc_part2->session,
                                         KRB5_KEYUSAGE_TGS_REQ_AUTH_CKSUM,
                                         source, his_cksum, &valid)))
        return(retval);

    if (!valid)
        return(KRB5KRB_AP_ERR_BAD_INTEGRITY);

    return(0);
}
