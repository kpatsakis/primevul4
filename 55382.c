EXPORTED char *mboxlist_find_specialuse(const char *use, const char *userid)
{
    /* \\Inbox is magical */
    if (!strcasecmp(use, "\\Inbox"))
        return mboxname_user_mbox(userid, NULL);

    struct _find_specialuse_data rock = { use, userid, NULL };
    mboxlist_usermboxtree(userid, _find_specialuse, &rock, MBOXTREE_SKIP_ROOT);
    return rock.mboxname;
}
