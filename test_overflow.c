/*
 * test_overflow.c - Test program for demonstrating integer overflow
 * 
 * This program demonstrates the integer overflow vulnerability
 * in the infusion pump code.
 */

#include <stdio.h>
#include <stdint.h>

// Performs the same calculation as in infusionvuln.c but with explicit uint32_t
uint32_t calculate_with_uint32(uint16_t rate, uint32_t duration_ms) {
    uint32_t ml_per_ms = (rate * 1000) / 3600000;
    
    if (ml_per_ms == 0) {
        uint32_t ml_per_5min = (rate * 300000) / 3600000;
        uint32_t volume_ml = (ml_per_5min * duration_ms) / 300000;
        return volume_ml * 1000; // Convert to microliters for consistency
    }
    
    uint32_t volume_ml = ml_per_ms * duration_ms;
    return volume_ml * 1000; // Convert to microliters for consistency
}

// Performs the calculation with uint64_t to show the correct result
uint64_t calculate_with_uint64(uint16_t rate, uint32_t duration_ms) {
    uint64_t volume_microliters = (uint64_t)rate * duration_ms * 1000 / 3600000;
    return volume_microliters;
}

int main() {
    uint16_t rate = 1000;  // 1000 ml/hr
    uint32_t safe_duration = 3600000;  // 1 hour
    uint32_t overflow_duration = 4294968;  // Just over 4,294,967 ms - chosen to trigger overflow
    
    printf("=== Testing with safe parameters ===\n");
    printf("Rate: %u ml/hr, Duration: %u ms\n", rate, safe_duration);
    
    uint32_t safe_result_32 = calculate_with_uint32(rate, safe_duration);
    uint64_t safe_result_64 = calculate_with_uint64(rate, safe_duration);
    
    printf("Result with uint32_t: %u microliters (%u ml)\n", 
           safe_result_32, safe_result_32 / 1000);
    printf("Result with uint64_t: %llu microliters (%llu ml)\n", 
           (unsigned long long)safe_result_64, (unsigned long long)safe_result_64 / 1000);
    
    printf("\n=== Testing with overflow parameters ===\n");
    printf("Rate: %u ml/hr, Duration: %u ms\n", rate, overflow_duration);
    printf("Intermediate calculation (rate * duration * 1000): %llu\n", 
           (unsigned long long)rate * overflow_duration * 1000);
    printf("Maximum uint32_t value: %u\n", UINT32_MAX);
    
    uint32_t overflow_result_32 = calculate_with_uint32(rate, overflow_duration);
    uint64_t overflow_result_64 = calculate_with_uint64(rate, overflow_duration);
    
    printf("Result with uint32_t (vulnerable): %u microliters (%u ml)\n", 
           overflow_result_32, overflow_result_32 / 1000);
    printf("Result with uint64_t (correct): %llu microliters (%llu ml)\n", 
           (unsigned long long)overflow_result_64, (unsigned long long)overflow_result_64 / 1000);
    
    // Show the magnitude of the difference
    printf("\nDiscrepancy due to overflow: %llu ml vs %u ml\n", 
           (unsigned long long)overflow_result_64 / 1000, overflow_result_32 / 1000);
    
    return 0;
}