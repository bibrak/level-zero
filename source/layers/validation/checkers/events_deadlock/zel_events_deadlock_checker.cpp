/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 * @file zel_eventsDeadlock_checker.cpp
 *
 */
#include "zel_eventsDeadlock_checker.h"

namespace validation_layer
{
    class eventsDeadlockChecker eventsDeadlock_checker;

    eventsDeadlockChecker::eventsDeadlockChecker() {
        enableeventsDeadlock = getenv_tobool( "ZEL_ENABLE_EVENTSDEADLOCK_CHECKER" );
        if(enableeventsDeadlock) {
            eventsDeadlockChecker::ZEeventsDeadlockChecker *zeChecker = new eventsDeadlockChecker::ZEeventsDeadlockChecker;
            eventsDeadlockChecker::ZESeventsDeadlockChecker *zesChecker = new eventsDeadlockChecker::ZESeventsDeadlockChecker;
            eventsDeadlockChecker::ZETeventsDeadlockChecker *zetChecker = new eventsDeadlockChecker::ZETeventsDeadlockChecker;
            eventsDeadlock_checker.zeValidation = zeChecker;
            eventsDeadlock_checker.zetValidation = zetChecker;
            eventsDeadlock_checker.zesValidation = zesChecker;
            validation_layer::context.validationHandlers.push_back(&eventsDeadlock_checker);
        }
    }

    eventsDeadlockChecker::~eventsDeadlockChecker() {
        if(enableeventsDeadlock) {
            delete eventsDeadlock_checker.zeValidation;
            delete eventsDeadlock_checker.zetValidation;
            delete eventsDeadlock_checker.zesValidation;
        }
    }
}