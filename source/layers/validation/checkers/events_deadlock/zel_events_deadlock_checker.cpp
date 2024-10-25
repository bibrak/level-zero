/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 * @file zel_events_deadlock_checker.cpp
 *
 */
#include "zel_events_deadlock_checker.h"
#include <iostream>

namespace validation_layer
{
    class eventsDeadlockChecker eventsDeadlock_checker;

    eventsDeadlockChecker::eventsDeadlockChecker()
    {
        enableEventsDeadlock = getenv_tobool("ZEL_ENABLE_EVENTSDEADLOCK_CHECKER");
        if (enableEventsDeadlock)
        {
            eventsDeadlockChecker::ZEeventsDeadlockChecker *zeChecker = new eventsDeadlockChecker::ZEeventsDeadlockChecker;
            // eventsDeadlockChecker::ZESeventsDeadlockChecker *zesChecker = new eventsDeadlockChecker::ZESeventsDeadlockChecker;
            // eventsDeadlockChecker::ZETeventsDeadlockChecker *zetChecker = new eventsDeadlockChecker::ZETeventsDeadlockChecker;
            eventsDeadlock_checker.zeValidation = zeChecker;
            // eventsDeadlock_checker.zetValidation = zetChecker;
            // eventsDeadlock_checker.zesValidation = zesChecker;
            validation_layer::context.validationHandlers.push_back(&eventsDeadlock_checker);
        }
    }

    eventsDeadlockChecker::~eventsDeadlockChecker()
    {
        if (enableEventsDeadlock)
        {
            delete eventsDeadlock_checker.zeValidation;
            // delete eventsDeadlock_checker.zetValidation;
            // delete eventsDeadlock_checker.zesValidation;
        }
    }

    ze_result_t
    eventsDeadlockChecker::ZEeventsDeadlockChecker::zeInitPrologue(
        ze_init_flags_t flags)
    {
        std::cout << "eventsDeadlockChecker: zeInitPrologue" << std::endl;
        return ZE_RESULT_SUCCESS;
    }

    ze_result_t eventsDeadlockChecker::ZEeventsDeadlockChecker::zeEventCreatePrologue(ze_event_pool_handle_t hEventPool, const ze_event_desc_t *desc, ze_event_handle_t *phEvent)
    {
        std::cout << "eventsDeadlockChecker: zeEventCreatePrologue" << std::endl;
        // std::cout << "\tphEvent->objMagic: " << (*phEvent)->objMagic << std::endl;
        printf("\tphEvent pointer address: %p\n", phEvent);
        printf("\t*phEvent pointer address: %p\n", *phEvent);

        // uint64_t temp = 20;
        // uint64_t twentytwo = 22;
        // memcpy(&temp, (void *)(&(*(*phEvent))), sizeof(uint64_t));
        // memcpy(&temp, &twentytwo, sizeof(uint64_t));
        // printf("\tphEvent->objMagic: %lu\n", temp);

        return ZE_RESULT_SUCCESS;
    }
    ze_result_t eventsDeadlockChecker::ZEeventsDeadlockChecker::zeEventCreateEpilogue(ze_event_pool_handle_t hEventPool, const ze_event_desc_t *desc, ze_event_handle_t *phEvent)
    {
        std::cout << "eventsDeadlockChecker: zeEventCreateEpilogue" << std::endl;
        // std::cout << "\tphEvent->objMagic: " << static_cast<uint64_t>(*(*phEvent)) << std::endl;
        printf("\tphEvent pointer address: %p\n", phEvent);
        printf("\t*phEvent pointer address: %p\n", *phEvent);
        return ZE_RESULT_SUCCESS;
    }

    ze_result_t
    eventsDeadlockChecker::ZEeventsDeadlockChecker::zeEventDestroyPrologue(
        ze_event_handle_t hEvent ///< [in][release] handle of event object to destroy
    )
    {
        std::cout << "eventsDeadlockChecker: zeEventDestroyPrologue" << std::endl;
        printf("\thEvent pointer address: %p\n", hEvent);
        // printf("\t*phEvent pointer address: %p\n", *phEvent);

        return ZE_RESULT_SUCCESS;
    }

    ze_result_t
    eventsDeadlockChecker::ZEeventsDeadlockChecker::zeEventDestroyEpilogue(
        ze_event_handle_t hEvent ///< [in][release] handle of event object to destroy
    )
    {
        std::cout << "eventsDeadlockChecker: zeEventDestroyEpilogue" << std::endl;
        printf("\thEvent pointer address: %p\n", hEvent);
        // printf("\t*phEvent pointer address: %p\n", *phEvent);

        return ZE_RESULT_SUCCESS;
    }

}