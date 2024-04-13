otp_edata(krb5_context context, krb5_kdc_req *request,
          krb5_kdcpreauth_callbacks cb, krb5_kdcpreauth_rock rock,
          krb5_kdcpreauth_moddata moddata, krb5_preauthtype pa_type,
          krb5_kdcpreauth_edata_respond_fn respond, void *arg)
{
    krb5_otp_tokeninfo ti, *tis[2] = { &ti, NULL };
    krb5_keyblock *armor_key = NULL;
    krb5_pa_otp_challenge chl;
    krb5_pa_data *pa = NULL;
    krb5_error_code retval;
    krb5_data *encoding;
    char *config;

    /* Determine if otp is enabled for the user. */
    retval = cb->get_string(context, rock, "otp", &config);
    if (retval == 0 && config == NULL)
        retval = ENOENT;
    if (retval != 0)
        goto out;
    cb->free_string(context, rock, config);

    /* Get the armor key.  This indicates the length of random data to use in
     * the nonce. */
    armor_key = cb->fast_armor(context, rock);
    if (armor_key == NULL) {
        retval = ENOENT;
        goto out;
    }

    /* Build the (mostly empty) challenge. */
    memset(&ti, 0, sizeof(ti));
    memset(&chl, 0, sizeof(chl));
    chl.tokeninfo = tis;
    ti.format = -1;
    ti.length = -1;
    ti.iteration_count = -1;

    /* Generate the nonce. */
    retval = nonce_generate(context, armor_key->length, &chl.nonce);
    if (retval != 0)
        goto out;

    /* Build the output pa-data. */
    retval = encode_krb5_pa_otp_challenge(&chl, &encoding);
    if (retval != 0)
        goto out;
    pa = k5alloc(sizeof(krb5_pa_data), &retval);
    if (pa == NULL) {
        krb5_free_data(context, encoding);
        goto out;
    }
    pa->pa_type = KRB5_PADATA_OTP_CHALLENGE;
    pa->contents = (krb5_octet *)encoding->data;
    pa->length = encoding->length;
    free(encoding);

out:
    (*respond)(arg, retval, pa);
}
