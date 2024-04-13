void dousers(const gchar *const username, const gchar *const groupname) {
	struct passwd *pw;
	struct group *gr;
	gchar* str;
	if (groupname) {
		gr = getgrnam(groupname);
		if(!gr) {
			str = g_strdup_printf("Invalid group name: %s", groupname);
			err(str);
		}
		if(setgid(gr->gr_gid)<0) {
			err("Could not set GID: %m"); 
		}
	}
	if (username) {
		pw = getpwnam(username);
		if(!pw) {
			str = g_strdup_printf("Invalid user name: %s", username);
			err(str);
		}
		if(setuid(pw->pw_uid)<0) {
			err("Could not set UID: %m");
		}
	}
}
