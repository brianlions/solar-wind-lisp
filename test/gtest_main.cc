/*
 * file name:           test/gtest_main.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Wed Nov 19 17:53:50 2014 CST
 */

/*
 * gtest_main.cc
 *
 *  Created on: Nov 24, 2011
 *  Author:     Brian Yi ZHANG
 *  Email:      brianlions@gmail.com
 */

#include <gtest/gtest.h>

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  int run_all_test_result = RUN_ALL_TESTS();

  return run_all_test_result;
}
