static bool test_writeclose(struct torture_context *tctx,
			    struct smbcli_state *cli)
{
	union smb_write io;
	NTSTATUS status;
	bool ret = true;
	int fnum;
	uint8_t *buf;
	const int maxsize = 90000;
	const char *fname = BASEDIR "\\test.txt";
	unsigned int seed = time(NULL);
	union smb_fileinfo finfo;

	buf = talloc_zero_array(tctx, uint8_t, maxsize);

	if (!torture_setting_bool(tctx, "writeclose_support", true)) {
		torture_skip(tctx, "Server does not support writeclose - skipping\n");
	}

	if (!torture_setup_dir(cli, BASEDIR)) {
		torture_fail(tctx, "failed to setup basedir");
	}

	torture_comment(tctx, "Testing RAW_WRITE_WRITECLOSE\n");
	io.generic.level = RAW_WRITE_WRITECLOSE;

	fnum = smbcli_open(cli->tree, fname, O_RDWR|O_CREAT, DENY_NONE);
	if (fnum == -1) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "Failed to create %s - %s\n", fname, smbcli_errstr(cli->tree)));
	}

	torture_comment(tctx, "Trying zero write\n");
	io.writeclose.in.file.fnum = fnum;
	io.writeclose.in.count = 0;
	io.writeclose.in.offset = 0;
	io.writeclose.in.mtime = 0;
	io.writeclose.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writeclose.out.nwritten, io.writeclose.in.count);

	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writeclose.out.nwritten, io.writeclose.in.count);

	setup_buffer(buf, seed, maxsize);

	torture_comment(tctx, "Trying small write\n");
	io.writeclose.in.count = 9;
	io.writeclose.in.offset = 4;
	io.writeclose.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);

	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_INVALID_HANDLE);

	fnum = smbcli_open(cli->tree, fname, O_RDWR, DENY_NONE);
	io.writeclose.in.file.fnum = fnum;

	if (smbcli_read(cli->tree, fnum, buf, 0, 13) != 13) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "read failed at %s\n", __location__));
	}
	CHECK_BUFFER(buf+4, seed, 9);
	CHECK_VALUE(IVAL(buf,0), 0);

	setup_buffer(buf, seed, maxsize);
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writeclose.out.nwritten, io.writeclose.in.count);

	fnum = smbcli_open(cli->tree, fname, O_RDWR, DENY_NONE);
	io.writeclose.in.file.fnum = fnum;

	memset(buf, 0, maxsize);
	if (smbcli_read(cli->tree, fnum, buf, 0, 13) != 13) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "read failed at %s\n", __location__));
	}
	CHECK_BUFFER(buf+4, seed, 9);
	CHECK_VALUE(IVAL(buf,0), 0);

	setup_buffer(buf, seed, maxsize);

	torture_comment(tctx, "Trying large write\n");
	io.writeclose.in.count = 4000;
	io.writeclose.in.offset = 0;
	io.writeclose.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writeclose.out.nwritten, 4000);

	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_INVALID_HANDLE);

	fnum = smbcli_open(cli->tree, fname, O_RDWR, DENY_NONE);
	io.writeclose.in.file.fnum = fnum;

	memset(buf, 0, maxsize);
	if (smbcli_read(cli->tree, fnum, buf, 0, 4000) != 4000) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "read failed at %s\n", __location__));
	}
	CHECK_BUFFER(buf, seed, 4000);

	torture_comment(tctx, "Trying bad fnum\n");
	io.writeclose.in.file.fnum = fnum+1;
	io.writeclose.in.count = 4000;
	io.writeclose.in.offset = 0;
	io.writeclose.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_INVALID_HANDLE);

	torture_comment(tctx, "Setting file as sparse\n");
	status = torture_set_sparse(cli->tree, fnum);
	CHECK_STATUS(status, NT_STATUS_OK);

	if (!(cli->transport->negotiate.capabilities & CAP_LARGE_FILES)) {
		torture_skip(tctx, "skipping large file tests - CAP_LARGE_FILES not set\n");
	}

	torture_comment(tctx, "Trying 2^32 offset\n");
	setup_buffer(buf, seed, maxsize);
	io.writeclose.in.file.fnum = fnum;
	io.writeclose.in.count = 4000;
	io.writeclose.in.offset = 0xFFFFFFFF - 2000;
	io.writeclose.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writeclose.out.nwritten, 4000);
	CHECK_ALL_INFO(io.writeclose.in.count + (uint64_t)io.writeclose.in.offset, size);

	fnum = smbcli_open(cli->tree, fname, O_RDWR, DENY_NONE);
	io.writeclose.in.file.fnum = fnum;

	memset(buf, 0, maxsize);
	if (smbcli_read(cli->tree, fnum, buf, io.writeclose.in.offset, 4000) != 4000) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "read failed at %s\n", __location__));
	}
	CHECK_BUFFER(buf, seed, 4000);

done:
	smbcli_close(cli->tree, fnum);
	smb_raw_exit(cli->session);
	smbcli_deltree(cli->tree, BASEDIR);
	return ret;
}