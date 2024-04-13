void ClientUsageTracker::GetHostUsage(
    const std::string& host, HostUsageCallback* callback) {
  HostSet::const_iterator found = cached_hosts_.find(host);
  if (found != cached_hosts_.end()) {
    callback->Run(host, type_, GetCachedHostUsage(host));
    delete callback;
    return;
  }
  if (!host_usage_callbacks_.Add(host, callback) || global_usage_task_)
    return;
  GatherHostUsageTask* task = new GatherHostUsageTask(tracker_, client_, host);
  host_usage_tasks_[host] = task;
  task->Start();
}
