#include "../src/llist.h"
#include <check.h>
#include <math.h>

#define POWER_UPPER_RANGE 10
START_TEST(Add_and_remove_test) {
    // Test all cases
    for (size_t power = 0; power <= POWER_UPPER_RANGE; power++) {
        // Alternate circular
        for (size_t circ = 0; circ <= 1; circ++) {
            // Running data
            int temp[1 << power];

            // Init list
            struct llist* subject = calloc(1, sizeof(struct llist));
            subject->circular = (bool) circ;
            subject->size = 1 << power;
            
            // Add pieces
            for (size_t index = 0; index < 1 << power; index++) {
                temp[index] = rand();
                ck_assert(llist_check(subject) >= E97_NONE);
                ck_assert(llist_append(subject, &temp[index]) >= E97_NONE);
                ck_assert(subject->size == index + 1);

            }

            // Mid check
            ck_assert(llist_check(subject) >= E97_NONE);

            // Remove pieces
            for (ssize_t index = (1 << power) - 1; index >= 0; index--) {
                int* returned;
                ck_assert(llist_pop(subject, &returned) >= E97_NONE);
                ck_assert(*returned == temp[index]);
                ck_assert(subject->size == index);
                ck_assert(llist_check(subject) >= E97_NONE);
            }

            // Size is correct
            ck_assert(subject->size >= E97_NONE);

            // Free up
            ck_assert(free_llist(subject, false) >= E97_NONE);
        }
    }
} END_TEST
