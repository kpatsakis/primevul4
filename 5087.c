static bool test_writex(struct torture_context *tctx,
			struct smbcli_state *cli)
{
	union smb_write io;
	NTSTATUS status;
	bool ret = true;
	int fnum, i;
	uint8_t *buf;
	const int maxsize = 90000;
	const char *fname = BASEDIR "\\test.txt";
	unsigned int seed = time(NULL);
	union smb_fileinfo finfo;
	int max_bits=63;

	if (!torture_setting_bool(tctx, "dangerous", false)) {
		max_bits=33;
		torture_comment(tctx, "dangerous not set - limiting range of test to 2^%d\n", max_bits);
	}

	buf = talloc_zero_array(tctx, uint8_t, maxsize);

	if (!cli->transport->negotiate.lockread_supported) {
		torture_comment(tctx, "Server does not support writeunlock - skipping\n");
		return true;
	}

	if (!torture_setup_dir(cli, BASEDIR)) {
		torture_fail(tctx, "failed to setup basedir");
	}

	torture_comment(tctx, "Testing RAW_WRITE_WRITEX\n");
	io.generic.level = RAW_WRITE_WRITEX;

	fnum = smbcli_open(cli->tree, fname, O_RDWR|O_CREAT, DENY_NONE);
	if (fnum == -1) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "Failed to create %s - %s\n", fname, smbcli_errstr(cli->tree)));
	}

	torture_comment(tctx, "Trying zero write\n");
	io.writex.in.file.fnum = fnum;
	io.writex.in.offset = 0;
	io.writex.in.wmode = 0;
	io.writex.in.remaining = 0;
	io.writex.in.count = 0;
	io.writex.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writex.out.nwritten, 0);

	setup_buffer(buf, seed, maxsize);

	torture_comment(tctx, "Trying small write\n");
	io.writex.in.count = 9;
	io.writex.in.offset = 4;
	io.writex.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writex.out.nwritten, io.writex.in.count);

	memset(buf, 0, maxsize);
	if (smbcli_read(cli->tree, fnum, buf, 0, 13) != 13) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "read failed at %s\n", __location__));
	}
	CHECK_BUFFER(buf+4, seed, 9);
	CHECK_VALUE(IVAL(buf,0), 0);

	setup_buffer(buf, seed, maxsize);

	torture_comment(tctx, "Trying large write\n");
	io.writex.in.count = 4000;
	io.writex.in.offset = 0;
	io.writex.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writex.out.nwritten, 4000);

	memset(buf, 0, maxsize);
	if (smbcli_read(cli->tree, fnum, buf, 0, 4000) != 4000) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "read failed at %s\n", __location__));
	}
	CHECK_BUFFER(buf, seed, 4000);

	torture_comment(tctx, "Trying bad fnum\n");
	io.writex.in.file.fnum = fnum+1;
	io.writex.in.count = 4000;
	io.writex.in.offset = 0;
	io.writex.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_INVALID_HANDLE);

	torture_comment(tctx, "Testing wmode\n");
	io.writex.in.file.fnum = fnum;
	io.writex.in.count = 1;
	io.writex.in.offset = 0;
	io.writex.in.wmode = 1;
	io.writex.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writex.out.nwritten, io.writex.in.count);

	io.writex.in.wmode = 2;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writex.out.nwritten, io.writex.in.count);


	torture_comment(tctx, "Trying locked region\n");
	cli->session->pid++;
	if (NT_STATUS_IS_ERR(smbcli_lock(cli->tree, fnum, 3, 1, 0, WRITE_LOCK))) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "Failed to lock file at %s\n", __location__));
	}
	cli->session->pid--;
	io.writex.in.wmode = 0;
	io.writex.in.count = 4;
	io.writex.in.offset = 0;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_FILE_LOCK_CONFLICT);

	torture_comment(tctx, "Setting file as sparse\n");
	status = torture_set_sparse(cli->tree, fnum);
	CHECK_STATUS(status, NT_STATUS_OK);

	if (!(cli->transport->negotiate.capabilities & CAP_LARGE_FILES)) {
		torture_skip(tctx, "skipping large file tests - CAP_LARGE_FILES not set\n");
	}

	torture_comment(tctx, "Trying 2^32 offset\n");
	setup_buffer(buf, seed, maxsize);
	io.writex.in.file.fnum = fnum;
	io.writex.in.count = 4000;
	io.writex.in.offset = 0xFFFFFFFF - 2000;
	io.writex.in.data = buf;
	status = smb_raw_write(cli->tree, &io);
	CHECK_STATUS(status, NT_STATUS_OK);
	CHECK_VALUE(io.writex.out.nwritten, 4000);
	CHECK_ALL_INFO(io.writex.in.count + (uint64_t)io.writex.in.offset, size);

	memset(buf, 0, maxsize);
	if (smbcli_read(cli->tree, fnum, buf, io.writex.in.offset, 4000) != 4000) {
		ret = false;
		torture_fail_goto(tctx, done, talloc_asprintf(tctx, "read failed at %s\n", __location__));
	}
	CHECK_BUFFER(buf, seed, 4000);

	for (i=33;i<max_bits;i++) {
		torture_comment(tctx, "Trying 2^%d offset\n", i);
		setup_buffer(buf, seed+1, maxsize);
		io.writex.in.file.fnum = fnum;
		io.writex.in.count = 4000;
		io.writex.in.offset = ((uint64_t)1) << i;
		io.writex.in.data = buf;
		status = smb_raw_write(cli->tree, &io);
		if (i>33 &&
		    NT_STATUS_EQUAL(status, NT_STATUS_INVALID_PARAMETER)) {
			break;
		}
		CHECK_STATUS(status, NT_STATUS_OK);
		CHECK_VALUE(io.writex.out.nwritten, 4000);
		CHECK_ALL_INFO(io.writex.in.count + (uint64_t)io.writex.in.offset, size);

		memset(buf, 0, maxsize);
		if (smbcli_read(cli->tree, fnum, buf, io.writex.in.offset, 4000) != 4000) {
			ret = false;
			torture_fail_goto(tctx, done, talloc_asprintf(tctx, "read failed at %s\n", __location__));
		}
		CHECK_BUFFER(buf, seed+1, 4000);
	}
	torture_comment(tctx, "limit is 2^%d\n", i);

	setup_buffer(buf, seed, maxsize);

done:
	smbcli_close(cli->tree, fnum);
	smb_raw_exit(cli->session);
	smbcli_deltree(cli->tree, BASEDIR);
	return ret;
}