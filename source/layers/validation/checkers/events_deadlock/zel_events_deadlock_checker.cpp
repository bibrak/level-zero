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

namespace validation_layer {
class eventsDeadlockChecker eventsDeadlock_checker;

eventsDeadlockChecker::eventsDeadlockChecker() {
    enableEventsDeadlock = getenv_tobool("ZEL_ENABLE_EVENTSDEADLOCK_CHECKER");
    if (enableEventsDeadlock) {
        eventsDeadlockChecker::ZEeventsDeadlockChecker *zeChecker = new eventsDeadlockChecker::ZEeventsDeadlockChecker;
        eventsDeadlock_checker.zeValidation = zeChecker;
        eventsDeadlock_checker.zesValidation = nullptr;
        eventsDeadlock_checker.zetValidation = nullptr;

        validation_layer::context.validationHandlers.push_back(&eventsDeadlock_checker);
    }
}

eventsDeadlockChecker::~eventsDeadlockChecker() {
    if (enableEventsDeadlock) {
        delete eventsDeadlock_checker.zeValidation;
    }
}

ze_result_t
eventsDeadlockChecker::ZEeventsDeadlockChecker::zeInitPrologue(
    ze_init_flags_t flags) {
    std::cout << "eventsDeadlockChecker: zeInitPrologue" << std::endl;
    return ZE_RESULT_SUCCESS;
}

ze_result_t eventsDeadlockChecker::ZEeventsDeadlockChecker::zeEventCreatePrologue(ze_event_pool_handle_t hEventPool, const ze_event_desc_t *desc, ze_event_handle_t *phEvent) {
    /* std::cout << "eventsDeadlockChecker: zeEventCreatePrologue" << std::endl;

    // printf("\tphEvent pointer address: %p\n", phEvent);
    printf("\t*phEvent pointer address: %p\n", *phEvent); */

    return ZE_RESULT_SUCCESS;
}
ze_result_t eventsDeadlockChecker::ZEeventsDeadlockChecker::zeEventCreateEpilogue(ze_event_pool_handle_t hEventPool, const ze_event_desc_t *desc, ze_event_handle_t *phEvent) {
    std::cout << "eventsDeadlockChecker: zeEventCreateEpilogue" << std::endl;

    // printf("\tphEvent pointer address: %p\n", phEvent);
    printf("\t*phEvent pointer address: %p\n", *phEvent);

    eventToDagID[*phEvent] = invalidDagID;

    return ZE_RESULT_SUCCESS;
}

ze_result_t
eventsDeadlockChecker::ZEeventsDeadlockChecker::zeEventDestroyPrologue(
    ze_event_handle_t hEvent ///< [in][release] handle of event object to destroy
) {
    std::cout << "eventsDeadlockChecker: zeEventDestroyPrologue" << std::endl;
    printf("\thEvent pointer address: %p\n", hEvent);

    return ZE_RESULT_SUCCESS;
}

ze_result_t
eventsDeadlockChecker::ZEeventsDeadlockChecker::zeEventDestroyEpilogue(
    ze_event_handle_t hEvent ///< [in][release] handle of event object to destroy
) {
    std::cout << "eventsDeadlockChecker: zeEventDestroyEpilogue" << std::endl;
    printf("\thEvent pointer address: %p\n", hEvent);

    if (eventToDagID.find(hEvent) != eventToDagID.end()) {
        printf("\t\tDeleted event from DAG\n");
        eventToDagID[hEvent] = invalidDagID;
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t
eventsDeadlockChecker::ZEeventsDeadlockChecker::zeCommandListAppendLaunchKernelPrologue(
    ze_command_list_handle_t hCommandList,   ///< [in] handle of the command list
    ze_kernel_handle_t hKernel,              ///< [in] handle of the kernel object
    const ze_group_count_t *pLaunchFuncArgs, ///< [in] thread group launch arguments
    ze_event_handle_t hSignalEvent,          ///< [in][optional] handle of the event to signal on completion
    uint32_t numWaitEvents,                  ///< [in][optional] number of events to wait on before launching; must be 0
                                             ///< if `nullptr == phWaitEvents`
    ze_event_handle_t *phWaitEvents          ///< [in][optional][range(0, numWaitEvents)] handle of the events to wait
                                             ///< on before launching
) {
    std::cout << "eventsDeadlockChecker: zeCommandListAppendLaunchKernelPrologue" << std::endl;
    printf("\tNumber of wait events: %d\n", numWaitEvents);
    for (uint32_t i = 0; i < numWaitEvents; i++) {
        printf("\tphWaitEvents[%d] pointer address: %p\n", i, phWaitEvents[i]);
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t
eventsDeadlockChecker::ZEeventsDeadlockChecker::zeCommandListAppendLaunchKernelEpilogue(
    ze_command_list_handle_t hCommandList,   ///< [in] handle of the command list
    ze_kernel_handle_t hKernel,              ///< [in] handle of the kernel object
    const ze_group_count_t *pLaunchFuncArgs, ///< [in] thread group launch arguments
    ze_event_handle_t hSignalEvent,          ///< [in][optional] handle of the event to signal on completion
    uint32_t numWaitEvents,                  ///< [in][optional] number of events to wait on before launching; must be 0
                                             ///< if `nullptr == phWaitEvents`
    ze_event_handle_t *phWaitEvents          ///< [in][optional][range(0, numWaitEvents)] handle of the events to wait
                                             ///< on before launching
) {
    std::cout << "eventsDeadlockChecker: zeCommandListAppendLaunchKernelEpilogue" << std::endl;
    printf("\tNumber of wait events: %d\n", numWaitEvents);
    for (uint32_t i = 0; i < numWaitEvents; i++) {
        printf("\tphWaitEvents[%d] pointer address: %p\n", i, phWaitEvents[i]);
    }
    // printf("\thEvent pointer address: %p\n", hEvent);

    return ZE_RESULT_SUCCESS;
}

ze_result_t
eventsDeadlockChecker::ZEeventsDeadlockChecker::zeCommandListAppendLaunchCooperativeKernelPrologue(
    ze_command_list_handle_t hCommandList,   ///< [in] handle of the command list
    ze_kernel_handle_t hKernel,              ///< [in] handle of the kernel object
    const ze_group_count_t *pLaunchFuncArgs, ///< [in] thread group launch arguments
    ze_event_handle_t hSignalEvent,          ///< [in][optional] handle of the event to signal on completion
    uint32_t numWaitEvents,                  ///< [in][optional] number of events to wait on before launching; must be 0
                                             ///< if `nullptr == phWaitEvents`
    ze_event_handle_t *phWaitEvents          ///< [in][optional][range(0, numWaitEvents)] handle of the events to wait
                                             ///< on before launching
) {

    return ZE_RESULT_SUCCESS;
}

ze_result_t
eventsDeadlockChecker::ZEeventsDeadlockChecker::zeCommandListAppendLaunchCooperativeKernelEpilogue(
    ze_command_list_handle_t hCommandList,   ///< [in] handle of the command list
    ze_kernel_handle_t hKernel,              ///< [in] handle of the kernel object
    const ze_group_count_t *pLaunchFuncArgs, ///< [in] thread group launch arguments
    ze_event_handle_t hSignalEvent,          ///< [in][optional] handle of the event to signal on completion
    uint32_t numWaitEvents,                  ///< [in][optional] number of events to wait on before launching; must be 0
                                             ///< if `nullptr == phWaitEvents`
    ze_event_handle_t *phWaitEvents          ///< [in][optional][range(0, numWaitEvents)] handle of the events to wait
                                             ///< on before launching
) {

    return ZE_RESULT_SUCCESS;
}

ze_result_t
eventsDeadlockChecker::ZEeventsDeadlockChecker::zeCommandListAppendMemoryCopyPrologue(
    ze_command_list_handle_t hCommandList, ///< [in] handle of command list
    void *dstptr,                          ///< [in] pointer to destination memory to copy to
    const void *srcptr,                    ///< [in] pointer to source memory to copy from
    size_t size,                           ///< [in] size in bytes to copy
    ze_event_handle_t hSignalEvent,        ///< [in][optional] handle of the event to signal on completion
    uint32_t numWaitEvents,                ///< [in][optional] number of events to wait on before launching; must be 0
                                           ///< if `nullptr == phWaitEvents`
    ze_event_handle_t *phWaitEvents        ///< [in][optional][range(0, numWaitEvents)] handle of the events to wait
                                           ///< on before launching
) {

    std::cout << "eventsDeadlockChecker: zeCommandListAppendMemoryCopyPrologue" << std::endl;
    printf("\t\tNumber of wait events: %d\n", numWaitEvents);
    for (uint32_t i = 0; i < numWaitEvents; i++) {
        printf("\t\tphWaitEvents[%d] pointer address: %p\n", i, phWaitEvents[i]);
    }

    int this_action_new_node_id = invalidDagID;

    if (hSignalEvent != nullptr) {

        if (eventToDagID.find(hSignalEvent) != eventToDagID.end()) {
            if (eventToDagID[hSignalEvent] != invalidDagID) {
                // This event already exists in the DAG. Get the DAG node ID.
                // For example when there is indeed a deadlock it would have already been created.
                this_action_new_node_id = eventToDagID[hSignalEvent];
            } else {
                // Create node in DAG
                this_action_new_node_id = nextDagID;
                nextDagID++;

                // Now we know where the hSignalEvent points from/out in the DAG. Update the eventtoDagID map.
                eventToDagID[hSignalEvent] = this_action_new_node_id;
                // std::cout << "\tUpdated eventToDagID: hSignalEvent = " << hSignalEvent << ", this_action_new_node_id = " << this_action_new_node_id << std::endl;
            }
        }
    }

    if (this_action_new_node_id == invalidDagID) {
        std::cerr << "eventsDeadlockChecker: zeCommandListAppendMemoryCopyPrologue: Error: this_action_new_node_id is invalid" << std::endl;
        std::terminate();
    }

    // Add this action to the actionToDagID map
    std::string action = "MemoryCopy";
    dagIDToAction[this_action_new_node_id] = actionAndSignalEvent(action, hSignalEvent);

    // Form the dependency in the DAG
    if (numWaitEvents != 0) {

        for (uint32_t i = 0; i < numWaitEvents; i++) {
            if (eventToDagID.find(phWaitEvents[i]) != eventToDagID.end()) {

                int dagID = eventToDagID[phWaitEvents[i]];
                if (dagID == invalidDagID) {
                    // Create a new node in the DAG for this wait event. That action will be created some time in the future.
                    int new_future_node_id = nextDagID;
                    nextDagID++;
                    eventToDagID[phWaitEvents[i]] = new_future_node_id;

                    dagID = new_future_node_id;
                }

                // Add edge (dependency) from dagID to this_action_new_node_id in the DAG.
                std::cout << "\tDAG: Adding edge from " << dagID << " to " << this_action_new_node_id << std::endl;
            } else {
                std::cerr << "eventsDeadlockChecker: zeCommandListAppendMemoryCopyPrologue: Error: Wait event not found in eventToDagID map" << std::endl;
                std::terminate();
            }
        }
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t
eventsDeadlockChecker::ZEeventsDeadlockChecker::zeCommandListAppendMemoryCopyEpilogue(
    ze_command_list_handle_t hCommandList, ///< [in] handle of command list
    void *dstptr,                          ///< [in] pointer to destination memory to copy to
    const void *srcptr,                    ///< [in] pointer to source memory to copy from
    size_t size,                           ///< [in] size in bytes to copy
    ze_event_handle_t hSignalEvent,        ///< [in][optional] handle of the event to signal on completion
    uint32_t numWaitEvents,                ///< [in][optional] number of events to wait on before launching; must be 0
                                           ///< if `nullptr == phWaitEvents`
    ze_event_handle_t *phWaitEvents        ///< [in][optional][range(0, numWaitEvents)] handle of the events to wait
                                           ///< on before launching
) {

    /*
    std::cout << "eventsDeadlockChecker: zeCommandListAppendMemoryCopyEpilogue" << std::endl;
    printf("\t\tNumber of wait events: %d\n", numWaitEvents);
    for (uint32_t i = 0; i < numWaitEvents; i++) {
        printf("\t\tphWaitEvents[%d] pointer address: %p\n", i, phWaitEvents[i]);
    }
    */
    return ZE_RESULT_SUCCESS;
}

} // namespace validation_layer