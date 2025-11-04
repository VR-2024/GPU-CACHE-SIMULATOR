#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_memory_trace(const char* filename, memory_trace_t** traces, uint32_t* count) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open trace file %s\n", filename);
        return -1;
    }

    *count = 0;
    char line[256];
    
    // First pass: count non-comment lines
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '#' && line[0] != '\n') (*count)++;
    }

    if (*count == 0) {
        fclose(file);
        printf("Error: Trace file is empty or contains only comments.\n");
        return -1;
    }

    *traces = (memory_trace_t*)malloc(*count * sizeof(memory_trace_t));
    if (!*traces) {
        fclose(file);
        printf("Error: Failed to allocate memory for traces.\n");
        return -1;
    }

    rewind(file);
    uint32_t idx = 0;
    
    // Second pass: parse data
    while (fgets(line, sizeof(line), file) && idx < *count) {
        if (line[0] == '#' || line[0] == '\n') continue;

        int result = sscanf(line, "%c %lx %u %u %u",
            &(*traces)[idx].operation,
            &(*traces)[idx].address,
            &(*traces)[idx].size,
            &(*traces)[idx].thread_id,
            &(*traces)[idx].block_id
        );

        if (result == 5) {
            idx++;
        } else {
            printf("Warning: Skipped invalid line %u in trace file: %s", idx + 1, line);
        }
    }

    *count = idx;
    fclose(file);
    return 0;
}

void free_memory_trace(memory_trace_t* traces) {
    if (traces) free(traces);
}

void print_trace_summary(memory_trace_t* traces, uint32_t count) {
    if (!traces || count == 0) return;

    uint32_t reads = 0, writes = 0;
    uint64_t min_addr = traces[0].address, max_addr = traces[0].address;

    for (uint32_t i = 0; i < count; i++) {
        if (traces[i].operation == 'R') reads++;
        else if (traces[i].operation == 'W') writes++;

        if (traces[i].address < min_addr) min_addr = traces[i].address;
        if (traces[i].address > max_addr) max_addr = traces[i].address;
    }

    printf("Memory Trace Summary:\n");
    printf("  Total Accesses: %u\n", count);
    printf("  Reads: %u (%.1f%%)\n", reads, (double)reads / count * 100.0);
    printf("  Writes: %u (%.1f%%)\n", writes, (double)writes / count * 100.0);
    printf("  Address Range: 0x%lx - 0x%lx\n\n", min_addr, max_addr);
}
