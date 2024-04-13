nonce_generate(krb5_context ctx, unsigned int length, krb5_data *nonce_out)
{
    krb5_data nonce;
    krb5_error_code retval;
    krb5_timestamp now;

    retval = krb5_timeofday(ctx, &now);
    if (retval != 0)
        return retval;

    retval = alloc_data(&nonce, sizeof(now) + length);
    if (retval != 0)
        return retval;

    retval = krb5_c_random_make_octets(ctx, &nonce);
    if (retval != 0) {
        free(nonce.data);
        return retval;
    }

    store_32_be(now, nonce.data);
    *nonce_out = nonce;
    return 0;
}
