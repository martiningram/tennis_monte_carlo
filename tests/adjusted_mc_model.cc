#include "adjusted_mc_model.h"

class AdjustedMCModelTest : public ::testing::Test {
 public:
  AdjustedMCModelTest()
      : data_(ModelData::ImportFromFile("tests/test_data.csv").front()),
        m_(data_.p1(), data_.p2(), true, data_) {}

 protected:
  ModelData data_;
  AdjustedMCModel m_;
};

TEST_F(AdjustedMCModelTest, IdentifiesGamePointsCorrectly) {
  Score test_score(data_.p1(), data_.p2());
  Point p(data_.p1(), data_.p2(), test_score);

  // 0-0:
  auto result = m_.IdentifyPoint(p);
  for (auto elt : result) {
    ASSERT_EQ(elt, false);
  }

  // 0-30, 15-30, 30-30:
  test_score.PlayerWinsPoint(data_.p2());
  test_score.PlayerWinsPoint(data_.p2());
  result = m_.IdentifyPoint(Point(data_.p1(), data_.p2(), test_score));

  std::array<bool, 5> expected{false, false, true, false, false};

  for (unsigned int j = 0; j < 2; ++j) {
    for (unsigned int i = 0; i < 5; ++i) {
      ASSERT_EQ(expected[i], result[i]);
    }
    test_score.PlayerWinsPoint(data_.p1());
    result = m_.IdentifyPoint(Point(data_.p1(), data_.p2(), test_score));
  }

  // 40-40:
  test_score.PlayerWinsPoint(data_.p1());
  test_score.PlayerWinsPoint(data_.p2());
  result = m_.IdentifyPoint(Point(data_.p1(), data_.p2(), test_score));

  expected = {false, false, true, false, false};

  for (unsigned int i = 0; i < 5; ++i) {
    ASSERT_EQ(expected[i], result[i]);
  }

  // 0-40, 15-40, 30-40
  // Reset:
  test_score = Score(data_.p1(), data_.p2());

  test_score.PlayerWinsPoint(data_.p2());
  test_score.PlayerWinsPoint(data_.p2());
  test_score.PlayerWinsPoint(data_.p2());

  result = m_.IdentifyPoint(Point(data_.p1(), data_.p2(), test_score));

  // TODO: Maybe add some code which ensures that nonsensical scores cannot be
  // created. Should be handled OK, but may be worth doing.

  expected = {false, true, false, false, false};

  for (unsigned int j = 0; j < 3; ++j) {
    for (unsigned int i = 0; i < 5; ++i) {
      ASSERT_EQ(expected[i], result[i]);
    }
    test_score.PlayerWinsPoint(data_.p1());
    result = m_.IdentifyPoint(Point(data_.p1(), data_.p2(), test_score));
  }

  // 40-AD:
  test_score.PlayerWinsPoint(data_.p2());

  result = m_.IdentifyPoint(Point(data_.p1(), data_.p2(), test_score));

  for (unsigned int i = 0; i < 5; ++i) {
    ASSERT_EQ(expected[i], result[i]);
  }
}
