  void verifyTransportParameters(std::chrono::milliseconds idleTimeout) {
    EXPECT_EQ(server->getConn().peerIdleTimeout, idleTimeout);
    if (getCanIgnorePathMTU()) {
      EXPECT_EQ(
          server->getConn().udpSendPacketLen, fakeHandshake->maxRecvPacketSize);
    }
  }