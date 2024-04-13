int _mkp_init(struct plugin_api **api, char *confdir)
{
    mk_api = *api;

    /* Init security lists */
    mk_list_init(&mk_secure_ip);
    mk_list_init(&mk_secure_url);
    mk_list_init(&mk_secure_deny_hotlink);

    /* Read configuration */
    mk_security_conf(confdir);
    return 0;
}
