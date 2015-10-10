#include "gtest/gtest.h"
#include "model_data.h"
#include <iostream>

bool IsClose(double d, double target) {
  double eps = 1E-3;

  double lower_bd = target - eps;
  double upper_bd = target + eps;

  return (d > lower_bd && d < upper_bd);
}

class ModelDataTest : public ::testing::Test {
 public:
  ModelDataTest()
      : model_data_(ModelData::ImportFromFile("tests/test_data.csv").front()),
        model_probs_p1_(model_data_.model_probs_p1()),
        model_probs_p2_(model_data_.model_probs_p2()) {}

 protected:
  ModelData model_data_;
  std::map<std::array<bool, 5>, double> model_probs_p1_;
  std::map<std::array<bool, 5>, double> model_probs_p2_;

  bool IsCorrect(std::array<bool, 5> situation, double p1, double p2) {
    double p1_prob = model_probs_p1_[situation];
    double p2_prob = model_probs_p2_[situation];
    return (IsClose(p1, p1_prob) && IsClose(p2, p2_prob));
  }
};

TEST_F(ModelDataTest, ReadCorrectly) {
  // Normal:
  std::array<bool, 5> situation{false, false, false, false, false};
  ASSERT_TRUE(IsCorrect(situation, 0.64313105, 0.68344134));

  // Tiebreak:
  situation = {true, false, false, false, false};
  ASSERT_TRUE(IsCorrect(situation, 0.63561062, 0.67694405));

  // Before BP:
  situation = {false, false, true, false, false};
  ASSERT_TRUE(IsCorrect(situation, 0.64392854, 0.67645446));

  // Set down:
  situation = {false, false, false, false, true};
  ASSERT_TRUE(IsCorrect(situation, 0.65288645, 0.68374495));

  // Set down & before BP:
  situation = {false, false, true, false, true};
  ASSERT_TRUE(IsCorrect(situation, 0.65367389, 0.67676160));

  // Set up:
  situation = {false, false, false, true, false};
  ASSERT_TRUE(IsCorrect(situation, 0.65163845, 0.68754710));

  // Set up & before BP:
  situation = {false, false, true, true, false};
  ASSERT_TRUE(IsCorrect(situation, 0.65242722, 0.68060849));

  // Set down & BP:
  situation = {false, true, false, false, true};
  ASSERT_TRUE(IsCorrect(situation, 0.64152962, 0.67923365));

  // Set up & BP:
  situation = {false, true, false, true, false};
  ASSERT_TRUE(IsCorrect(situation, 0.64026329, 0.68306490));

  // BP:
  situation = {false, true, false, false, false};
  ASSERT_TRUE(IsCorrect(situation, 0.63163512, 0.67892774));

  // Set up & TB:
  situation = {true, false, false, true, false};
  ASSERT_TRUE(IsCorrect(situation, 0.64419813, 0.68109475));

  // Set down & TB:
  situation = {true, false, false, false, true};
  ASSERT_TRUE(IsCorrect(situation, 0.64545827, 0.67725094));
}
