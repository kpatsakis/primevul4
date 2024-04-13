  void checkTransportStateUpdate(
      const std::shared_ptr<FileQLogger>& qLogger,
      const std::string& msg) {
    std::vector<int> indices =
        getQLogEventIndices(QLogEventType::TransportStateUpdate, qLogger);
    EXPECT_EQ(indices.size(), 1);
    auto tmp = std::move(qLogger->logs[indices[0]]);
    auto event = dynamic_cast<QLogTransportStateUpdateEvent*>(tmp.get());
    EXPECT_EQ(event->update, getPeerClose(msg));
  }