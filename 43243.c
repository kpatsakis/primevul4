get_policy(kadm5_server_handle_t handle, const char *name,
           kadm5_policy_ent_t policy_out, krb5_boolean *have_pol_out)
{
    kadm5_ret_t ret;

    *have_pol_out = FALSE;
    if (name == NULL)
        return 0;
    ret = kadm5_get_policy(handle->lhandle, (char *)name, policy_out);
    if (ret == 0)
        *have_pol_out = TRUE;
    return (ret == KADM5_UNK_POLICY) ? 0 : ret;
}
