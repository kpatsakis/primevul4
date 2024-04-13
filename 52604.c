rep_etypes2str(char *s, size_t len, krb5_kdc_rep *rep)
{
    char stmp[sizeof("ses=") + D_LEN(krb5_enctype)];

    if (len < (3 * D_LEN(krb5_enctype)
               + sizeof("etypes {rep= tkt= ses=}"))) {
        *s = '\0';
        return;
    }

    snprintf(s, len, "etypes {rep=%ld", (long)rep->enc_part.enctype);

    if (rep->ticket != NULL) {
        snprintf(stmp, sizeof(stmp),
                 " tkt=%ld", (long)rep->ticket->enc_part.enctype);
        strlcat(s, stmp, len);
    }

    if (rep->ticket != NULL
        && rep->ticket->enc_part2 != NULL
        && rep->ticket->enc_part2->session != NULL) {
        snprintf(stmp, sizeof(stmp), " ses=%ld",
                 (long)rep->ticket->enc_part2->session->enctype);
        strlcat(s, stmp, len);
    }
    strlcat(s, "}", len);
    return;
}
