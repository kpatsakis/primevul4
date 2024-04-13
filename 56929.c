sms_error_handler (SmsConn       conn,
                   Bool          swap,
                   int           offending_minor_opcode,
                   unsigned long offending_sequence_num,
                   int           error_class,
                   int           severity,
                   IcePointer    values)
{
        g_debug ("GsmXsmpServer: sms_error_handler (%p, %s, %d, %lx, %d, %d)",
                 conn, swap ? "TRUE" : "FALSE", offending_minor_opcode,
                 offending_sequence_num, error_class, severity);

        /* We don't need to do anything here; if the connection needs to be
         * closed, libSM will do that itself.
         */
}
