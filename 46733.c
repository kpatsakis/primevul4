state_show(struct md_rdev *rdev, char *page)
{
	char *sep = "";
	size_t len = 0;
	unsigned long flags = ACCESS_ONCE(rdev->flags);

	if (test_bit(Faulty, &flags) ||
	    rdev->badblocks.unacked_exist) {
		len+= sprintf(page+len, "%sfaulty",sep);
		sep = ",";
	}
	if (test_bit(In_sync, &flags)) {
		len += sprintf(page+len, "%sin_sync",sep);
		sep = ",";
	}
	if (test_bit(WriteMostly, &flags)) {
		len += sprintf(page+len, "%swrite_mostly",sep);
		sep = ",";
	}
	if (test_bit(Blocked, &flags) ||
	    (rdev->badblocks.unacked_exist
	     && !test_bit(Faulty, &flags))) {
		len += sprintf(page+len, "%sblocked", sep);
		sep = ",";
	}
	if (!test_bit(Faulty, &flags) &&
	    !test_bit(In_sync, &flags)) {
		len += sprintf(page+len, "%sspare", sep);
		sep = ",";
	}
	if (test_bit(WriteErrorSeen, &flags)) {
		len += sprintf(page+len, "%swrite_error", sep);
		sep = ",";
	}
	if (test_bit(WantReplacement, &flags)) {
		len += sprintf(page+len, "%swant_replacement", sep);
		sep = ",";
	}
	if (test_bit(Replacement, &flags)) {
		len += sprintf(page+len, "%sreplacement", sep);
		sep = ",";
	}

	return len+sprintf(page+len, "\n");
}
