static int racls_add_cb(const mbentry_t *mbentry, void *rock)
{
    struct txn **txn = (struct txn **)rock;
    return mboxlist_update_racl(mbentry->name, NULL, mbentry, txn);
}
