#pragma once

#include "../Geometry.h"
#include <cmath>
#include <gtest/gtest.h>
#include <pthread.h>


bool is_equal(double x, double y) {
  return std::abs(x - y) < (std::numeric_limits<float>::epsilon() * std::abs(x + y)) || 
         std::abs(x-y) < std::numeric_limits<float>::min();
}

TEST(Vectors3, AdditionTest) {
  Vector3 vec_1 = {1.0, 2.0, 3.0}, vec_2 = {2.0, 3.0, 4.0};
  Vector3 res = vec_1 + vec_2;
  ASSERT_TRUE(is_equal(res.x, 3.0) && is_equal(res.y, 5.0) && is_equal(res.z, 7.0)); 
}

TEST(Vectors3, SubtractionTest) {
  Vector3 vec_1 = {1.0, 2.0, 3.0}, vec_2 = {2.0, 3.0, 4.0};
  Vector3 res = vec_2 - vec_1;
  ASSERT_TRUE(is_equal(res.x, 1.0) && is_equal(res.y, 1.0) && is_equal(res.z, 1.0)); 
}

TEST(Vectors3, MultTest) {
  Vector3 vec_1 = {1.0, 2.0, 3.0};
  float numerator = 2;
  Vector3 res = vec_1 * numerator;
  ASSERT_TRUE(is_equal(res.x, 2.0) && is_equal(res.y, 4.0) && is_equal(res.z, 6.0)); 
}

TEST(Vectors3, distTest) {
  Vector3 vec_1 = {1.0, 2.0, 3.0};
  double res = std::sqrt(1.0 + 4.0 + 9.0);
  ASSERT_TRUE(is_equal(Vector3::Distance(vec_1), res)); 
}

TEST(Vectors3, NormalTest) {
  Vector3 vec_1 = {1.0, 2.0, 3.0};
  vec_1.Normalise();
  ASSERT_TRUE(is_equal(Vector3::Distance(vec_1), 1.0f)); 
}
//------ Vector2 tests
TEST(Vectors2, AdditionTest) {
  Vector2 vec_1 = {1.0, 2.0}, vec_2 = {2.0, 3.0};
  Vector2 res = vec_1 + vec_2;
  ASSERT_TRUE(is_equal(res.x, 3.0) && is_equal(res.y, 5.0)); 
}

TEST(Vectors2, SubtractionTest) {
  Vector2 vec_1 = {1.0, 2.0}, vec_2 = {2.0, 3.0};
  Vector2 res = vec_2 - vec_1;
  ASSERT_TRUE(is_equal(res.x, 1.0) && is_equal(res.y, 1.0)); 
}

TEST(Vectors2, MultTest) {
  Vector2 vec_1 = {1.0, 2.0};
  float numerator = 2;
  Vector2 res = vec_1 * numerator;
  ASSERT_TRUE(is_equal(res.x, 2.0) && is_equal(res.y, 4.0)); 
}

TEST(Vectors2, distTest) {
  Vector2 vec_1 = {1.0, 2.0};
  double res = std::sqrt(1.0 + 4.0);
  ASSERT_TRUE(is_equal(Vector2::Distance(vec_1), res)); 
}

TEST(Vectors2, NormalTest) {
  Vector2 vec_1 = {1.0, 2.0};
  vec_1.Normalise();
  ASSERT_TRUE(is_equal(Vector2::Distance(vec_1), 1.0f)); 
}