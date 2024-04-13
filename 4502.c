TEST_F(QuicUnencryptedServerTransportTest, DuplicateOneRttWriteCipher) {
  setupClientReadCodec();
  recvClientHello();
  recvClientFinished();
  loopForWrites();
  try {
    recvClientHello();
    recvClientFinished();
    FAIL();
  } catch (const std::runtime_error& ex) {
    EXPECT_THAT(ex.what(), HasSubstr("Crypto error"));
  }
  EXPECT_TRUE(server->isClosed());
}