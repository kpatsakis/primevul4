TEST_F(QuicServerTransportTest, TestRegisterAndHandleTransportKnobParams) {
  int flag = 0;
  server->registerKnobParamHandler(
      199, [&](QuicServerConnectionState* /* server_conn */, uint64_t val) {
        EXPECT_EQ(val, 10);
        flag = 1;
      });
  server->registerKnobParamHandler(
      200,
      [&](QuicServerConnectionState* /* server_conn */, uint64_t /* val */) {
        flag = 2;
      });
  server->handleKnobParams({
      {199, 10},
      {201, 20},
  });

  EXPECT_EQ(flag, 1);

  // ovewrite will fail, the new handler won't be called
  server->registerKnobParamHandler(
      199, [&](QuicServerConnectionState* /* server_conn */, uint64_t val) {
        EXPECT_EQ(val, 30);
        flag = 3;
      });

  server->handleKnobParams({
      {199, 10},
      {201, 20},
  });
  EXPECT_EQ(flag, 1);
}