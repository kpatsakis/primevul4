  GatherGlobalUsageTask(
      UsageTracker* tracker,
      QuotaClient* client)
      : GatherUsageTaskBase(tracker, client),
        client_(client),
        callback_factory_(ALLOW_THIS_IN_INITIALIZER_LIST(this)) {
    DCHECK(tracker);
    DCHECK(client);
  }
