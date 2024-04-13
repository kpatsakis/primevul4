pkinit_pkcs11_code_to_text(int err)
{
    int i;
    static char uc[32];

    for (i = 0; pkcs11_errstrings[i].text != NULL; i++)
        if (pkcs11_errstrings[i].code == err)
            break;
    if (pkcs11_errstrings[i].text != NULL)
        return (pkcs11_errstrings[i].text);
    snprintf(uc, sizeof(uc), _("unknown code 0x%x"), err);
    return (uc);
}
