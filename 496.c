  GatherUsageTaskBase(
      UsageTracker* tracker,
      QuotaClient* client)
      : QuotaTask(tracker),
        client_(client),
        tracker_(tracker),
        callback_factory_(ALLOW_THIS_IN_INITIALIZER_LIST(this)) {
    DCHECK(tracker_);
    DCHECK(client_);
    client_tracker_ = tracker_->GetClientTracker(client_->id());
    DCHECK(client_tracker_);
  }
