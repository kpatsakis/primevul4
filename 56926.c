ice_error_handler (IceConn       conn,
                   Bool          swap,
                   int           offending_minor_opcode,
                   unsigned long offending_sequence,
                   int           error_class,
                   int           severity,
                   IcePointer    values)
{
        g_debug ("GsmXsmpServer: ice_error_handler (%p, %s, %d, %lx, %d, %d)",
                 conn, swap ? "TRUE" : "FALSE", offending_minor_opcode,
                 offending_sequence, error_class, severity);

        if (severity == IceCanContinue) {
                return;
        }

        /* FIXME: the ICElib docs are completely vague about what we're
         * supposed to do in this case. Need to verify that calling
         * IceCloseConnection() here is guaranteed to cause neither
         * free-memory-reads nor leaks.
         */
        IceCloseConnection (conn);
}
