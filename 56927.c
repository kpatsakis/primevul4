ice_io_error_handler (IceConn conn)
{
        g_debug ("GsmXsmpServer: ice_io_error_handler (%p)", conn);

        /* We don't need to do anything here; the next call to
         * IceProcessMessages() for this connection will receive
         * IceProcessMessagesIOError and we can handle the error there.
         */
}
