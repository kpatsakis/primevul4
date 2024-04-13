timestamp_verify(krb5_context ctx, const krb5_data *nonce)
{
    krb5_error_code retval = EINVAL;
    krb5_pa_enc_ts *et = NULL;

    if (nonce->data == NULL)
        goto out;

    /* Decode the PA-ENC-TS-ENC structure. */
    retval = decode_krb5_pa_enc_ts(nonce, &et);
    if (retval != 0)
        goto out;

    /* Check the clockskew. */
    retval = krb5_check_clockskew(ctx, et->patimestamp);

out:
    krb5_free_pa_enc_ts(ctx, et);
    return retval;
}
