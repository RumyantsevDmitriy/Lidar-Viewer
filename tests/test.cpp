#include <gtest/gtest.h>

#include <pthread.h>

#include "test.hpp"

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  //::testing::InitGoogleMock(&argc, argv);
  
  return RUN_ALL_TESTS();
}