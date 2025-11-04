#include "gpu_memory_system.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_usage(const char* prog) {
    printf("Usage: %s <trace_file>\n", prog);
    printf("\nExample: %s data/memory_trace.txt\n", prog);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    printf("GPU Cache & Memory Hierarchy Simulator\n");
    printf("======================================\n\n");

    memory_trace_t* traces = NULL;
    uint32_t trace_count = 0;

    if (load_memory_trace(argv[1], &traces, &trace_count) != 0) return 1;

    printf("Loaded %u memory accesses from %s\n", trace_count, argv[1]);
    print_trace_summary(traces, trace_count);

    gpu_memory_system_t* system = create_gpu_memory_system();
    if (!system) {
        printf("Error: Failed to create GPU memory system.\n");
        free_memory_trace(traces);
        return 1;
    }

    printf("Running simulation...\n");

    clock_t start_time = clock();

    for (uint32_t i = 0; i < trace_count; i++) {
        memory_trace_t* trace = &traces[i];
        
        memory_access_t access = {
            .address = trace->address,
            .type = (trace->operation == 'W') ? ACCESS_WRITE : ACCESS_READ,
            // Ensure thread/block IDs are within bounds
            .thread_id = trace->thread_id % MAX_THREADS, 
            .block_id = trace->block_id % MAX_BLOCKS 
        };

        // Note: Latency calculation is simplified in gpu_memory_access
        uint32_t latency = gpu_memory_access(system, &access);
        system->current_cycle += latency;
        
        if ((i + 1) % 100 == 0) {
            printf(" Processed: %u/%u accesses\r", i + 1, trace_count);
            fflush(stdout);
        }
    }

    clock_t end_time = clock();
    double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("\nSimulation completed in %.2f seconds\n", elapsed);

    print_gpu_system_stats(system);

    printf("\nSimulation Performance:\n");
    printf(" Simulation Speed: %.2f accesses/second\n\n", trace_count / elapsed);

    free_memory_trace(traces);
    free_gpu_memory_system(system);

    return 0;
}
