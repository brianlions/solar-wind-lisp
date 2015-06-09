/*
 * file name:           test/gtest_main.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Wed Nov 19 17:53:50 2014 CST
 */

#include <gtest/gtest.h>
#include <protobuf/stubs/common.h>

int main(int argc, char ** argv)
{
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  testing::InitGoogleTest(&argc, argv);
  int run_all_test_result = RUN_ALL_TESTS();
  google::protobuf::ShutdownProtobufLibrary();

  return run_all_test_result;
}
