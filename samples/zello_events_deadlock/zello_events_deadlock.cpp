/*
 *
 * Copyright (C) 2020-2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */
#include "zello_init.h"

#include <memory>
#include <stdlib.h>

void print_loader_versions() {

    zel_component_version_t *versions;
    size_t size = 0;
    zelLoaderGetVersions(&size, nullptr);
    std::cout << "zelLoaderGetVersions number of components found: " << size << std::endl;
    versions = new zel_component_version_t[size];
    zelLoaderGetVersions(&size, versions);

    for (size_t i = 0; i < size; i++) {
        std::cout << "Version " << i << std::endl;
        std::cout << "Name: " << versions[i].component_name << std::endl;
        std::cout << "Major: " << versions[i].component_lib_version.major << std::endl;
        std::cout << "Minor: " << versions[i].component_lib_version.minor << std::endl;
        std::cout << "Patch: " << versions[i].component_lib_version.patch << std::endl;
    }

    delete[] versions;
}

#if defined(_WIN32)
#define putenv_safe _putenv
#else
#define putenv_safe putenv
#endif

template <bool TerminateOnFailure, typename ResulT>
inline void validate(ResulT result, const char *message) {
    if (result != 0) { /* assumption 0 is success */
        std::cerr << (TerminateOnFailure ? "ERROR : " : "WARNING : ") << message
                  << " : " << result << std::endl;
        if (TerminateOnFailure) {
            std::terminate();
        }
        return;
    }
    constexpr bool verbose = true;
    if (verbose) {
        std::cerr << " SUCCESS : " << message << std::endl;
    }
}

#define SUCCESS_OR_TERMINATE(CALL) validate<true>(CALL, #CALL)

//////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
    bool tracing_runtime_enabled = false;
    if (argparse(argc, argv, "-null", "--enable_null_driver")) {
        putenv_safe(const_cast<char *>("ZE_ENABLE_NULL_DRIVER=1"));
    }
    if (argparse(argc, argv, "-ldr", "--force_loader_intercepts")) {
        putenv_safe(const_cast<char *>("ZE_ENABLE_LOADER_INTERCEPT=1"));
    }
    if (argparse(argc, argv, "-val", "--enable_validation_layer")) {
        putenv_safe(const_cast<char *>("ZE_ENABLE_VALIDATION_LAYER=1"));
        putenv_safe(const_cast<char *>("ZE_ENABLE_PARAMETER_VALIDATION=1"));
    }
    if (argparse(argc, argv, "-trace", "--enable_tracing_layer")) {
        putenv_safe(const_cast<char *>("ZE_ENABLE_TRACING_LAYER=1"));
    }
    if (argparse(argc, argv, "-tracerun", "--enable_tracing_layer_runtime")) {
        tracing_runtime_enabled = true;
    }

    ze_result_t status;
    const ze_device_type_t type = ZE_DEVICE_TYPE_GPU;

    ze_driver_handle_t pDriver = nullptr;
    ze_device_handle_t pDevice = nullptr;
    if (init_ze()) {

        print_loader_versions();

        if (tracing_runtime_enabled) {
            std::cout << "Enabling Tracing Layer after init" << std::endl;
            status = zelEnableTracingLayer();
            if (status != ZE_RESULT_SUCCESS) {
                std::cout << "zelEnableTracingLayer Failed with return code: " << to_string(status) << std::endl;
                exit(1);
            }
        }

        uint32_t driverCount = 0;
        status = zeDriverGet(&driverCount, nullptr);
        if (status != ZE_RESULT_SUCCESS) {
            std::cout << "zeDriverGet Failed with return code: " << to_string(status) << std::endl;
            exit(1);
        }

        std::vector<ze_driver_handle_t> drivers(driverCount);
        status = zeDriverGet(&driverCount, drivers.data());
        if (status != ZE_RESULT_SUCCESS) {
            std::cout << "zeDriverGet Failed with return code: " << to_string(status) << std::endl;
            exit(1);
        }

        for (uint32_t driver = 0; driver < driverCount; ++driver) {
            pDriver = drivers[driver];
            pDevice = findDevice(pDriver, type);
            if (pDevice) {
                break;
            }
        }
    }

    if (!pDevice) {
        std::cout << "Did NOT find matching " << to_string(type) << " device!" << "\n";
        return -1;
    }

    // Create the context
    ze_context_handle_t context;
    ze_context_desc_t context_desc = {};
    context_desc.stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC;
    status = zeContextCreate(pDriver, &context_desc, &context);
    if (status != ZE_RESULT_SUCCESS) {
        std::cout << "zeContextCreate Failed with return code: " << to_string(status) << std::endl;
        exit(1);
    }

    // Create an immediate command list for direct submission
    ze_command_queue_desc_t altdesc = {};
    altdesc.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC;
    ze_command_list_handle_t command_list = {};
    status = zeCommandListCreateImmediate(context, pDevice, &altdesc, &command_list);
    if (status != ZE_RESULT_SUCCESS) {
        std::cout << "zeCommandListCreateImmediate Failed with return code: " << to_string(status) << std::endl;
        exit(1);
    }

    // Create an event to be signaled by the device
    ze_event_pool_desc_t ep_desc = {};
    ep_desc.stype = ZE_STRUCTURE_TYPE_EVENT_POOL_DESC;
    ep_desc.count = 4;
    ep_desc.flags = ZE_EVENT_POOL_FLAG_HOST_VISIBLE;

    ze_event_pool_handle_t event_pool;

    status = zeEventPoolCreate(context, &ep_desc, 1, &pDevice, &event_pool);
    if (status != ZE_RESULT_SUCCESS) {
        std::cout << "zeEventPoolCreate Failed with return code: " << to_string(status) << std::endl;
        exit(1);
    }

    std::vector<ze_event_handle_t> event{};
    // Two events for memcpy that will form a dependency on a 3rd event
    event.resize(3);

    ze_event_desc_t ev_desc = {};
    ev_desc.stype = ZE_STRUCTURE_TYPE_EVENT_DESC;
    ev_desc.signal = ZE_EVENT_SCOPE_FLAG_DEVICE; // ZE_EVENT_SCOPE_FLAG_HOST ??
    ev_desc.wait = ZE_EVENT_SCOPE_FLAG_HOST;

    ev_desc.index = 0;
    SUCCESS_OR_TERMINATE(zeEventCreate(event_pool, &ev_desc, &event[0]));

    ev_desc.index++;
    SUCCESS_OR_TERMINATE(zeEventCreate(event_pool, &ev_desc, &event[1]));

    ev_desc.index++;
    SUCCESS_OR_TERMINATE(zeEventCreate(event_pool, &ev_desc, &event[2]));

    /* ev_desc.signal = ZE_EVENT_SCOPE_FLAG_HOST;
    ev_desc.wait = ZE_EVENT_SCOPE_FLAG_DEVICE;
    ev_desc.index++;
    ze_event_handle_t start_event;
    SUCCESS_OR_TERMINATE(zeEventCreate(event_pool, &ev_desc, &start_event)); */

    ze_host_mem_alloc_desc_t host_desc = {};
    host_desc.stype = ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC;
    host_desc.pNext = nullptr;
    host_desc.flags = 0;

    size_t buffer_size = 1024;
    void *host_mem_ptr = nullptr;
    SUCCESS_OR_TERMINATE(zeMemAllocHost(context, &host_desc, buffer_size, 1, &host_mem_ptr));

    ze_device_mem_alloc_desc_t device_desc = {};
    device_desc.stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC;
    device_desc.pNext = nullptr;
    device_desc.ordinal = 0;
    device_desc.flags = 0;


    void *device_mem_ptr = nullptr;
    SUCCESS_OR_TERMINATE(zeMemAllocDevice(context, &device_desc, buffer_size, 0, pDevice, &device_mem_ptr));

    std::cout << "\n\n\n";

    // Action_0: Host to Device
    SUCCESS_OR_TERMINATE(zeCommandListAppendMemoryCopy(command_list, device_mem_ptr, host_mem_ptr, buffer_size, event[0], 1 /* 1 */, &event[2] /* &start_event */));

    // Action_1: Host to Device, is dependent on Action_0
    SUCCESS_OR_TERMINATE(zeCommandListAppendMemoryCopy(command_list, device_mem_ptr, host_mem_ptr, buffer_size, event[1], 1, &event[0]));

    // Action_2: Host to Device, is dependent on Action_1
    SUCCESS_OR_TERMINATE(zeCommandListAppendMemoryCopy(command_list, device_mem_ptr, host_mem_ptr, buffer_size, event[2], 1, &event[1]));

    // Create the event deadlock by having event[2] dependent on Action_0
    // TODO!!! It will be created when creating Action_0 and specifying event[2] as the dependent event

    std::cout << "\n\n\n";

    SUCCESS_OR_TERMINATE(zeCommandListClose(command_list));

    ze_command_queue_desc_t command_queue_description{};
    command_queue_description.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC;
    command_queue_description.pNext = nullptr;
    command_queue_description.ordinal = 0;
    command_queue_description.index = 0;
    command_queue_description.mode = ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS;

    ze_command_queue_handle_t command_queue{};
    SUCCESS_OR_TERMINATE(zeCommandQueueCreate(context, pDevice, &command_queue_description, &command_queue));

    // This segfaults. TODO!!! Fix
    // SUCCESS_OR_TERMINATE(zeCommandQueueExecuteCommandLists(command_queue, 1, &command_list, nullptr));

    SUCCESS_OR_TERMINATE(zeCommandQueueSynchronize(command_queue, UINT64_MAX));

    // SUCCESS_OR_TERMINATE(zeEventHostSignal(start_event));

    // signal the event from the device and wait for completion

    // zeCommandListAppendSignalEvent(command_list, event[0]);
    // zeEventHostSynchronize(event[0], UINT64_MAX);

    std::cout << "Congratulations, the device completed execution!\n";

    SUCCESS_OR_TERMINATE(zeCommandQueueDestroy(command_queue));

    // These two hang. TODO!!! Fix
    /* SUCCESS_OR_TERMINATE(zeMemFree(context, host_mem_ptr));
    SUCCESS_OR_TERMINATE(zeMemFree(context, device_mem_ptr)); */

    SUCCESS_OR_TERMINATE(zeEventDestroy(event[0]));
    SUCCESS_OR_TERMINATE(zeEventDestroy(event[1]));
    SUCCESS_OR_TERMINATE(zeEventDestroy(event[2]));
    // SUCCESS_OR_TERMINATE(zeEventDestroy(start_event));

    // These these hang. TODO!!! Fix
    /* SUCCESS_OR_TERMINATE(zeEventPoolDestroy(event_pool));
    SUCCESS_OR_TERMINATE(zeCommandListDestroy(command_list));

    SUCCESS_OR_TERMINATE(zeContextDestroy(context));*/

    if (tracing_runtime_enabled) {
        std::cout << "Disable Tracing Layer after init" << std::endl;
        status = zelDisableTracingLayer();
        if (status != ZE_RESULT_SUCCESS) {
            std::cout << "zelDisableTracingLayer Failed with return code: " << to_string(status) << std::endl;
            exit(1);
        }
    }
    std::cout << "Returning with 0 looks like it hangs here ... ???" << std::endl;
    return 0;
}