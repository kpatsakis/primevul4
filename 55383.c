EXPORTED char *mboxlist_find_uniqueid(const char *uniqueid, const char *userid)
{
    struct _find_uniqueid_data rock = { uniqueid, NULL };
    mboxlist_usermboxtree(userid, _find_uniqueid, &rock, MBOXTREE_PLUS_RACL);
    return rock.mboxname;
}
