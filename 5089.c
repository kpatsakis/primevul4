static bool test_writeunlock(struct torture_context *tctx,
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

	if (!cli->transport->negotiate.lockread_supported) {
		torture_skip(tctx, "Server does not support writeunlock - skipping\n");
	}

	if (!torture_setup_dir(cli, BASEDIR)) {
		torture_fail(tctx, "failed to setup basedir");
	}

	torture_comment(tctx, "Testing RAW_WRITE_WRITEUNLOCK\n");
	io.generic.level = RAW_WRITE_WRITEUNLOCK;

	fnum = smbcli_open(cli->tree, fname, O_RDWR|O_CREAT, DENY_NONE);
	if (fnum == -1) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "Failed to create %s - %s\n", fname, smbcli_errstr(cli->tree)));
	}

	torture_comment(tctx, "Trying zero write\n");
	io.writeunlock.in.file.fnum = fnum;
	io.writeunlock.in.count = 0;
	io.writeunlock.in.offset = 0;
	io.writeunlock.in.remaining = 0;
	io.writeunlock.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writeunlock.out.nwritten, io.writeunlock.in.count);

	setup_buffer(buf, seed, maxsize);

	torture_comment(tctx, "Trying small write\n");
	io.writeunlock.in.count = 9;
	io.writeunlock.in.offset = 4;
	io.writeunlock.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_RANGE_NOT_LOCKED);
	if (smbcli_read(cli->tree, fnum, buf, 0, 13) != 13) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "read failed at %s\n", __location__));
	}
	CHECK_BUFFER(buf+4, seed, 9);
	CHECK_VALUE(IVAL(buf,0), 0);

	setup_buffer(buf, seed, maxsize);
	smbcli_lock(cli->tree, fnum, io.writeunlock.in.offset, io.writeunlock.in.count,
		 0, WRITE_LOCK);
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writeunlock.out.nwritten, io.writeunlock.in.count);

	memset(buf, 0, maxsize);
	if (smbcli_read(cli->tree, fnum, buf, 0, 13) != 13) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "read failed at %s\n", __location__));
	}
	CHECK_BUFFER(buf+4, seed, 9);
	CHECK_VALUE(IVAL(buf,0), 0);

	setup_buffer(buf, seed, maxsize);

	torture_comment(tctx, "Trying large write\n");
	io.writeunlock.in.count = 4000;
	io.writeunlock.in.offset = 0;
	io.writeunlock.in.data = buf;
	smbcli_lock(cli->tree, fnum, io.writeunlock.in.offset, io.writeunlock.in.count,
		 0, WRITE_LOCK);
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writeunlock.out.nwritten, 4000);

	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_RANGE_NOT_LOCKED);

	memset(buf, 0, maxsize);
	if (smbcli_read(cli->tree, fnum, buf, 0, 4000) != 4000) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "read failed at %s\n", __location__));
	}
	CHECK_BUFFER(buf, seed, 4000);

	torture_comment(tctx, "Trying bad fnum\n");
	io.writeunlock.in.file.fnum = fnum+1;
	io.writeunlock.in.count = 4000;
	io.writeunlock.in.offset = 0;
	io.writeunlock.in.data = buf;
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
	io.writeunlock.in.file.fnum = fnum;
	io.writeunlock.in.count = 4000;
	io.writeunlock.in.offset = 0xFFFFFFFF - 2000;
	io.writeunlock.in.data = buf;
	smbcli_lock(cli->tree, fnum, io.writeunlock.in.offset, io.writeunlock.in.count,
		 0, WRITE_LOCK);
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writeunlock.out.nwritten, 4000);
	CHECK_ALL_INFO(io.writeunlock.in.count + (uint64_t)io.writeunlock.in.offset, size);

	memset(buf, 0, maxsize);
	if (smbcli_read(cli->tree, fnum, buf, io.writeunlock.in.offset, 4000) != 4000) {
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