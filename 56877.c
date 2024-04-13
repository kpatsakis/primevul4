static void remove_notification(struct mqueue_inode_info *info)
{
	if (info->notify_owner != NULL &&
	    info->notify.sigev_notify == SIGEV_THREAD) {
		set_cookie(info->notify_cookie, NOTIFY_REMOVED);
		netlink_sendskb(info->notify_sock, info->notify_cookie);
	}
	put_pid(info->notify_owner);
	put_user_ns(info->notify_user_ns);
	info->notify_owner = NULL;
	info->notify_user_ns = NULL;
}
