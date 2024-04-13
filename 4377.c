TEST_P(QuicServerTransportHandshakeTest, TestD6DStartCallback) {
  Observer::Config config = {};
  config.pmtuEvents = true;
  auto mockObserver = std::make_unique<MockObserver>(config);
  server->addObserver(mockObserver.get());
  // Set oneRttReader so that maybeStartD6DPriobing passes its check
  auto codec = std::make_unique<QuicReadCodec>(QuicNodeType::Server);
  codec->setOneRttReadCipher(createNoOpAead());
  server->getNonConstConn().readCodec = std::move(codec);
  // And the state too
  server->getNonConstConn().d6d.state = D6DMachineState::BASE;
  EXPECT_CALL(*mockObserver, pmtuProbingStarted(_)).Times(1);
  // CHLO should be enough to trigger probing
  recvClientHello();
  server->removeObserver(mockObserver.get());
}