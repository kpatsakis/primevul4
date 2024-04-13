save_yourself_request_callback (SmsConn   conn,
                                SmPointer manager_data,
                                int       save_type,
                                Bool      shutdown,
                                int       interact_style,
                                Bool      fast,
                                Bool      global)
{
        GsmXSMPClient *client = manager_data;

        g_debug ("GsmXSMPClient: Client '%s' received SaveYourselfRequest(%s, %s, %s, %s, %s)",
                 client->priv->description,
                 save_type == SmSaveLocal ? "SmSaveLocal" :
                 save_type == SmSaveGlobal ? "SmSaveGlobal" : "SmSaveBoth",
                 shutdown ? "Shutdown" : "!Shutdown",
                 interact_style == SmInteractStyleAny ? "SmInteractStyleAny" :
                 interact_style == SmInteractStyleErrors ? "SmInteractStyleErrors" :
                 "SmInteractStyleNone", fast ? "Fast" : "!Fast",
                 global ? "Global" : "!Global");

        /* Examining the g_debug above, you can see that there are a total
         * of 72 different combinations of options that this could have been
         * called with. However, most of them are stupid.
         *
         * If @shutdown and @global are both TRUE, that means the caller is
         * requesting that a logout message be sent to all clients, so we do
         * that. We use @fast to decide whether or not to show a
         * confirmation dialog. (This isn't really what @fast is for, but
         * the old gnome-session and ksmserver both interpret it that way,
         * so we do too.) We ignore @save_type because we pick the correct
         * save_type ourselves later based on user prefs, dialog choices,
         * etc, and we ignore @interact_style, because clients have not used
         * it correctly consistently enough to make it worth honoring.
         *
         * If @shutdown is TRUE and @global is FALSE, the caller is
         * confused, so we ignore the request.
         *
         * If @shutdown is FALSE and @save_type is SmSaveGlobal or
         * SmSaveBoth, then the client wants us to ask some or all open
         * applications to save open files to disk, but NOT quit. This is
         * silly and so we ignore the request.
         *
         * If @shutdown is FALSE and @save_type is SmSaveLocal, then the
         * client wants us to ask some or all open applications to update
         * their current saved state, but not log out. At the moment, the
         * code only supports this for the !global case (ie, a client
         * requesting that it be allowed to update *its own* saved state,
         * but not having everyone else update their saved state).
         */

        if (shutdown && global) {
                g_debug ("GsmXSMPClient:   initiating shutdown");
                g_signal_emit (client, signals[LOGOUT_REQUEST], 0, !fast);
        } else if (!shutdown && !global) {
                g_debug ("GsmXSMPClient:   initiating checkpoint");
                do_save_yourself (client, SmSaveLocal, TRUE);
        } else {
                g_debug ("GsmXSMPClient:   ignoring");
        }
}
