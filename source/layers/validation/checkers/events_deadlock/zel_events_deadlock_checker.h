/*
 *
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 * @file zel_events_deadlock_checker.h
 *
 */

#pragma once

#include <string>
#include "ze_api.h"
#include "ze_validation_layer.h"

namespace validation_layer
{
    class __zedlllocal eventsDeadlockChecker : public validationChecker
    {
    public:
        eventsDeadlockChecker();
        ~eventsDeadlockChecker();

        class ZEeventsDeadlockChecker : public ZEValidationEntryPoints
        {
        public:
            ze_result_t zeInitPrologue(ze_init_flags_t flags) override;
            ze_result_t zeEventCreatePrologue(ze_event_pool_handle_t hEventPool, const ze_event_desc_t *desc, ze_event_handle_t *phEvent) override;
            ze_result_t zeEventCreateEpilogue(ze_event_pool_handle_t hEventPool, const ze_event_desc_t *desc, ze_event_handle_t *phEvent) override;
            ze_result_t zeEventDestroyPrologue(ze_event_handle_t hEvent) override;
            ze_result_t zeEventDestroyEpilogue(ze_event_handle_t hEvent) override;
        };
        // class ZESeventsDeadlockChecker : public ZESValidationEntryPoints {};
        // class ZETeventsDeadlockChecker : public ZETValidationEntryPoints {};
        bool enableEventsDeadlock = false;
    };
    extern class eventsDeadlockChecker eventsDeadlock_checker;
}