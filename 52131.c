iperf_has_zerocopy( void )
{
    return has_sendfile();
}
