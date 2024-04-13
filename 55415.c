int parseentry_cb(int type, struct dlistsax_data *d)
{
    struct parseentry_rock *rock = (struct parseentry_rock *)d->rock;

    switch(type) {
    case DLISTSAX_KVLISTSTART:
        if (!strcmp(buf_cstring(&d->kbuf), "A")) {
            rock->doingacl = 1;
        }
        break;
    case DLISTSAX_KVLISTEND:
        rock->doingacl = 0;
        break;
    case DLISTSAX_STRING:
        if (rock->doingacl) {
            buf_append(rock->aclbuf, &d->kbuf);
            buf_putc(rock->aclbuf, '\t');
            buf_append(rock->aclbuf, &d->buf);
            buf_putc(rock->aclbuf, '\t');
        }
        else {
            const char *key = buf_cstring(&d->kbuf);
            if (!strcmp(key, "F")) {
                rock->mbentry->foldermodseq = atoll(buf_cstring(&d->buf));
            }
            else if (!strcmp(key, "I")) {
                rock->mbentry->uniqueid = buf_newcstring(&d->buf);
            }
            else if (!strcmp(key, "M")) {
                rock->mbentry->mtime = atoi(buf_cstring(&d->buf));
            }
            else if (!strcmp(key, "P")) {
                rock->mbentry->partition = buf_newcstring(&d->buf);
            }
            else if (!strcmp(key, "S")) {
                rock->mbentry->server = buf_newcstring(&d->buf);
            }
            else if (!strcmp(key, "T")) {
                rock->mbentry->mbtype = mboxlist_string_to_mbtype(buf_cstring(&d->buf));
            }
            else if (!strcmp(key, "V")) {
                rock->mbentry->uidvalidity = atol(buf_cstring(&d->buf));
            }
        }
    }

    return 0;
}
