static void send_reply(uint32_t opt, int net, uint32_t reply_type, size_t datasize, void* data) {
	uint64_t magic = htonll(0x3e889045565a9LL);
	reply_type = htonl(reply_type);
	uint32_t datsize = htonl(datasize);
	struct iovec v_data[] = {
		{ &magic, sizeof(magic) },
		{ &opt, sizeof(opt) },
		{ &reply_type, sizeof(reply_type) },
		{ &datsize, sizeof(datsize) },
		{ data, datasize },
	};
	writev(net, v_data, 5);
}
