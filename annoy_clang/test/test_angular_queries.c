//
// Created by Mark Tamarov (7amarov.mark@gmail.com) on 24.06.2025.
//

#include "test_support.h"

int main(void) {
    AnnoyIndexHandle index = BuildBasicAngularIndex();

    int by_item[3];
    GetNnsByItem(index, 0, 3, -1, by_item);
    assert(by_item[0] == 0);

    float query[] = {1.0f, 0.2f, 0.2f};
    int by_vector[3];
    GetNnsByVector(index, query, 3, by_vector);
    assert(by_vector[0] == 0);

    AssertFloatNear(GetDistance(index, 0, 1), 1.4142135f, 0.0001f);

    float item_buffer[3];
    float expected_item[] = {0.0f, 1.0f, 0.0f};
    GetItem(index, 1, item_buffer);
    AssertFloatArrayEquals(item_buffer, expected_item, 3, 0.0001f);

    Free(index);
    return 0;
}
