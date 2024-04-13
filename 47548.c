decrypt_encdata(krb5_context context, krb5_keyblock *armor_key,
                krb5_pa_otp_req *req, krb5_data *out)
{
    krb5_error_code retval;
    krb5_data plaintext;

    if (req == NULL)
        return EINVAL;

    retval = alloc_data(&plaintext, req->enc_data.ciphertext.length);
    if (retval)
        return retval;

    retval = krb5_c_decrypt(context, armor_key, KRB5_KEYUSAGE_PA_OTP_REQUEST,
                            NULL, &req->enc_data, &plaintext);
    if (retval != 0) {
        com_err("otp", retval, "Unable to decrypt encData in PA-OTP-REQUEST");
        free(plaintext.data);
        return retval;
    }

    *out = plaintext;
    return 0;
}
