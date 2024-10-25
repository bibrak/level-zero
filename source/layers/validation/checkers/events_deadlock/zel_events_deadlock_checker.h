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

#include "ze_api.h"
#include "ze_validation_layer.h"

#include <string>

namespace validation_layer {
class __zedlllocal eventsDeadlockChecker : public validationChecker {
  public:
    eventsDeadlockChecker();
    ~eventsDeadlockChecker();

    class ZEeventsDeadlockChecker : public ZEValidationEntryPoints {
      public:
        ze_result_t zeInitPrologue(ze_init_flags_t flags) override;
        ze_result_t zeEventCreatePrologue(ze_event_pool_handle_t hEventPool, const ze_event_desc_t *desc, ze_event_handle_t *phEvent) override;
        ze_result_t zeEventCreateEpilogue(ze_event_pool_handle_t hEventPool, const ze_event_desc_t *desc, ze_event_handle_t *phEvent) override;
        ze_result_t zeEventDestroyPrologue(ze_event_handle_t hEvent) override;
        ze_result_t zeEventDestroyEpilogue(ze_event_handle_t hEvent) override;
        ze_result_t zeCommandListAppendLaunchKernelPrologue(ze_command_list_handle_t hCommandList, ze_kernel_handle_t hKernel, const ze_group_count_t *pLaunchFuncArgs, ze_event_handle_t hSignalEvent, uint32_t numWaitEvents, ze_event_handle_t *phWaitEvents) override;
        ze_result_t zeCommandListAppendLaunchKernelEpilogue(ze_command_list_handle_t hCommandList, ze_kernel_handle_t hKernel, const ze_group_count_t *pLaunchFuncArgs, ze_event_handle_t hSignalEvent, uint32_t numWaitEvents, ze_event_handle_t *phWaitEvents) override;
        ze_result_t zeCommandListAppendLaunchCooperativeKernelPrologue(ze_command_list_handle_t hCommandList, ze_kernel_handle_t hKernel, const ze_group_count_t *pLaunchFuncArgs, ze_event_handle_t hSignalEvent, uint32_t numWaitEvents, ze_event_handle_t *phWaitEvents) override;
        ze_result_t zeCommandListAppendLaunchCooperativeKernelEpilogue(ze_command_list_handle_t hCommandList, ze_kernel_handle_t hKernel, const ze_group_count_t *pLaunchFuncArgs, ze_event_handle_t hSignalEvent, uint32_t numWaitEvents, ze_event_handle_t *phWaitEvents) override;
        ze_result_t zeCommandListAppendMemoryCopyPrologue(ze_command_list_handle_t hCommandList, void *dstptr, const void *srcptr, size_t size, ze_event_handle_t hSignalEvent, uint32_t numWaitEvents, ze_event_handle_t *phWaitEvents) override;
        ze_result_t zeCommandListAppendMemoryCopyEpilogue(ze_command_list_handle_t hCommandList, void *dstptr, const void *srcptr, size_t size, ze_event_handle_t hSignalEvent, uint32_t numWaitEvents, ze_event_handle_t *phWaitEvents) override;
    };
    // class ZESeventsDeadlockChecker : public ZESValidationEntryPoints {};
    // class ZETeventsDeadlockChecker : public ZETValidationEntryPoints {};
    bool enableEventsDeadlock = false;
};
extern class eventsDeadlockChecker eventsDeadlock_checker;
} // namespace validation_layer