#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

// Should it be here?
// Can we change initialVector?
// TEST(AsDataFlow, ChangesInitialVector) {
//     std::vector<int> input = {1, 2, 3, 4, 5};
//     auto result = AsDataFlow(input);

//     for(auto& item : result) {
//       item += 2;
//     }

//     ASSERT_THAT(input, testing::ElementsAre(3, 4, 5, 6, 7));
// }