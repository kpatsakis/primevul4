int _mkp_stage_10(unsigned int socket, struct sched_connection *conx)
{
    (void) conx;

    /* Validate ip address with Mandril rules */
    if (mk_security_check_ip(socket) != 0) {
        PLUGIN_TRACE("[FD %i] Mandril close connection", socket);
        return MK_PLUGIN_RET_CLOSE_CONX;
    }
    return MK_PLUGIN_RET_CONTINUE;
}
