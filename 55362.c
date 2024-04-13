EXPORTED int mboxlist_alluser(user_cb *proc, void *rock)
{
    struct alluser_rock urock;
    int r = 0;
    urock.prev = NULL;
    urock.proc = proc;
    urock.rock = rock;
    r = mboxlist_allmbox(NULL, alluser_cb, &urock, /*flags*/0);
    free(urock.prev);
    return r;
}
