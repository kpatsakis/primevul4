static void list_add_connection(struct mt_connection *conn) {
	DL_APPEND(connections_head, conn);
}
