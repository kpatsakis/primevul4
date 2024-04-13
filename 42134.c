pkinit_login(krb5_context context,
             pkinit_identity_crypto_context id_cryptoctx,
             CK_TOKEN_INFO *tip)
{
    krb5_data rdat;
    char *prompt;
    const char *warning;
    krb5_prompt kprompt;
    krb5_prompt_type prompt_type;
    int r = 0;

    if (tip->flags & CKF_PROTECTED_AUTHENTICATION_PATH) {
        rdat.data = NULL;
        rdat.length = 0;
    } else {
        if (tip->flags & CKF_USER_PIN_LOCKED)
            warning = " (Warning: PIN locked)";
        else if (tip->flags & CKF_USER_PIN_FINAL_TRY)
            warning = " (Warning: PIN final try)";
        else if (tip->flags & CKF_USER_PIN_COUNT_LOW)
            warning = " (Warning: PIN count low)";
        else
            warning = "";
        if (asprintf(&prompt, "%.*s PIN%s", (int) sizeof (tip->label),
                     tip->label, warning) < 0)
            return ENOMEM;
        rdat.data = malloc(tip->ulMaxPinLen + 2);
        rdat.length = tip->ulMaxPinLen + 1;

        kprompt.prompt = prompt;
        kprompt.hidden = 1;
        kprompt.reply = &rdat;
        prompt_type = KRB5_PROMPT_TYPE_PREAUTH;

        /* PROMPTER_INVOCATION */
        k5int_set_prompt_types(context, &prompt_type);
        r = (*id_cryptoctx->prompter)(context, id_cryptoctx->prompter_data,
                                      NULL, NULL, 1, &kprompt);
        k5int_set_prompt_types(context, 0);
        free(prompt);
    }

    if (r == 0) {
        r = id_cryptoctx->p11->C_Login(id_cryptoctx->session, CKU_USER,
                                       (u_char *) rdat.data, rdat.length);

        if (r != CKR_OK) {
            pkiDebug("C_Login: %s\n", pkinit_pkcs11_code_to_text(r));
            r = KRB5KDC_ERR_PREAUTH_FAILED;
        }
    }
    free(rdat.data);

    return r;
}
