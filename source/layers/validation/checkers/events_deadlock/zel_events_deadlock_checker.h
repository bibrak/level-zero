/*
 *
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 * @file zel_eventsDeadlock_checker.h
 *
 */

#pragma once 

#include <string>
#include "ze_api.h"
#include "ze_validation_layer.h"

namespace validation_layer
{
    class __zedlllocal eventsDeadlockChecker : public validationChecker{
        public:
            eventsDeadlockChecker();
            ~eventsDeadlockChecker();

            class ZEeventsDeadlockChecker : public ZEValidationEntryPoints {};
            class ZESeventsDeadlockChecker : public ZESValidationEntryPoints {};
            class ZETeventsDeadlockChecker : public ZETValidationEntryPoints {};
        bool enableeventsDeadlock = false;
    };
    extern class eventsDeadlockChecker eventsDeadlock_checker;
}