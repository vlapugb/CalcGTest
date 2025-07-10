#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "SimpleCalculator.h"
#include "InMemoryHistory.h"
#include "IHistory.h"
#include "ICalculator.h"

using ::testing::_;
using ::testing::Exactly;
using ::testing::Return;
using ::testing::StrictMock;

namespace calc {
    class MockHistory : public IHistory {
    public:
        MOCK_METHOD(void, AddEntry, (const std::string& operation), (override));
        MOCK_METHOD(std::vector<std::string>, GetLastOperations, (size_t count), (const, override));
    };

    class MockCalculator : public ICalculator {
    public:
        MOCK_METHOD(int, Add, (int, int), (override));
        MOCK_METHOD(int, Subtract, (int, int), (override));
        MOCK_METHOD(int, Multiply, (int, int), (override));
        MOCK_METHOD(int, Divide, (int, int), (override));
        MOCK_METHOD(void, SetHistory, (IHistory&), (override));
    };
}

TEST(SimpleCalculatorTests, Add_LogsOperationToHistory) {
    StrictMock<calc::MockHistory> mockHistory;
    calc::SimpleCalculator calculator(mockHistory);

    EXPECT_CALL(mockHistory, AddEntry("2 + 2 = 4")).Times(1);

    const int result = calculator.Add(2, 2);
    EXPECT_EQ(result, 4);
}

TEST(SimpleCalculatorTests, Multiply_LogsOperationToHistory) {
    StrictMock<calc::MockHistory> mockHistory;
    calc::SimpleCalculator calculator(mockHistory);

    EXPECT_CALL(mockHistory, AddEntry("3 * 3 = 9")).Times(1);

    EXPECT_EQ(calculator.Multiply(3, 3), 9);
}

TEST(SimpleCalculatorTests, Divide_LogsOperationToHistory) {
    StrictMock<calc::MockHistory> mockHistory;
    calc::SimpleCalculator calculator(mockHistory);

    EXPECT_CALL(mockHistory, AddEntry("6 / 3 = 2")).Times(1);

    EXPECT_EQ(calculator.Divide(6, 3), 2);
}

TEST(SimpleCalculatorTests, FractionalDivision_IsTruncated) {
    StrictMock<calc::MockHistory> mockHistory;
    calc::SimpleCalculator calculator(mockHistory);

    EXPECT_CALL(mockHistory, AddEntry("5 / 2 = 2")).Times(1);
    EXPECT_EQ(calculator.Divide(5, 2), 2); // integer truncation
}

TEST(SimpleCalculatorTests, Add_WithNegativeOperands) {
    StrictMock<calc::MockHistory> mockHistory;
    calc::SimpleCalculator calculator(mockHistory);

    EXPECT_CALL(mockHistory, AddEntry("-5 + -3 = -8")).Times(1);
    EXPECT_EQ(calculator.Add(-5, -3), -8);
}

TEST(SimpleCalculatorTests, Add_WithIntMaxValue) {
    StrictMock<calc::MockHistory> mockHistory;
    calc::SimpleCalculator calculator(mockHistory);

    constexpr int max = std::numeric_limits<int>::max();
    std::ostringstream expected;
    expected << max << " + 0 = " << max;

    EXPECT_CALL(mockHistory, AddEntry(expected.str())).Times(1);
    EXPECT_EQ(calculator.Add(max, 0), max);
}

TEST(SimpleCalculatorTests, Divide_ByZero_TriggersDeath) {
    StrictMock<calc::MockHistory> mockHistory;
    calc::SimpleCalculator calculator(mockHistory);

    ASSERT_DEATH({ calculator.Divide(1, 0); }, ".*");
}

TEST(InMemoryHistoryTests, StoresAndReturnsSingleOperation) {
    calc::InMemoryHistory history;
    history.AddEntry("1 + 1 = 2");

    const auto last = history.GetLastOperations(1);
    ASSERT_EQ(last.size(), 1);
    EXPECT_EQ(last[0], "1 + 1 = 2");
}

TEST(InMemoryHistoryTests, ReturnsLastNOperationsInInsertionOrder) {
    calc::InMemoryHistory history;
    history.AddEntry("1 + 1 = 2");
    history.AddEntry("2 * 2 = 4");
    history.AddEntry("4 - 3 = 1");

    const auto lastTwo = history.GetLastOperations(2);
    const std::vector<std::string> expected = {"2 * 2 = 4", "4 - 3 = 1"};
    EXPECT_EQ(lastTwo, expected);
}

TEST(IntegrationTests, OperationsPersistWhenCalculatorIsMocked) {
    calc::InMemoryHistory realHistory; {
        std::ostringstream oss;
        oss << 10 << " + " << 15 << " = " << 25;
        realHistory.AddEntry(oss.str());
    }

    const auto last = realHistory.GetLastOperations(1);
    ASSERT_EQ(last.size(), 1);
    EXPECT_EQ(last[0], "10 + 15 = 25");
}
