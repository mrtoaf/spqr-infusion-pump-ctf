# SPQR Infusion Pump CTF - Solution Guide

This document provides a detailed explanation of the vulnerability in the infusion pump firmware and how to exploit it.

## Vulnerability Overview

The infusion pump firmware contains an **integer overflow vulnerability** in the `calculate_volume_to_deliver` function. This vulnerability allows an attacker to cause the pump to deliver an incorrect amount of medication, which could be dangerous or fatal in a real medical device.

## Technical Analysis

### Vulnerable Function

The vulnerable function is `calculate_volume_to_deliver()` on line 71 of `infusionvuln.c`:

```c
uint16_t calculate_volume_to_deliver(uint32_t time_increment_ms) {
    // Calculate volume to deliver during this time increment (in microliters for precision)
    // Formula: (rate_ml_per_hour * time_increment_ms) / (3600 * 1000)
    
    uint32_t volume_microliters = pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;
    
    // Convert back to milliliters (truncating fractions)
    uint16_t volume_ml = (uint16_t)(volume_microliters / 1000);
    
    return volume_ml;
}
```

### The Vulnerability

The vulnerability is on line 77:

```c
uint32_t volume_microliters = pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;
```

This line performs a calculation that's vulnerable to integer overflow. Let's break down what happens:

1. `pump_state.rate_ml_per_hour` is a `uint16_t` (max value: 65,535)
2. `time_increment_ms` is a `uint32_t` (max value: 4,294,967,295)
3. The calculation multiplies these values and then multiplies by 1000

If the intermediate result of `pump_state.rate_ml_per_hour * time_increment_ms * 1000` exceeds the maximum value of a `uint32_t` (4,294,967,295), an integer overflow occurs. When this happens, the result "wraps around" to a much smaller value.

### Exploitation Path

The vulnerability can be exploited using the `admin_set_custom_parameters()` function, which allows setting arbitrary values for `rate` and `duration_ms` without safety checks.

## Exploitation Steps

1. First, access the admin panel by:
   - Looking at the source code to find the hard-coded password: `MedSecure2023`
   - Selecting option 7 (Admin Access) and entering this password

2. Next, find values that will cause the intermediate calculation to exceed 2^32 (4,294,967,296):
   - In the admin menu, select option 1 (Set Custom Parameters)
   - Set `rate = 1000` (the maximum allowed rate)
   - Set `duration_ms = 4294968` (just over 4,294,967, enough to cause overflow)

3. Then run the simulation with these values:
   - In the admin menu, select option 2 (Run Full Simulation)
   - Enter the same duration value (4294968)

4. Observe the results:
   - The calculation shows: 1000 * 4294968 * 1000 = 4,294,968,000,000 (which exceeds 2^32)
   - Expected delivery: approximately 1193 ml
   - Actual delivery: 0 ml (due to integer overflow)

## Real-World Impact

In a real infusion pump:

1. **Underdosing scenario**: The pump would appear to be working correctly, but the patient would receive little or no medication. For critical medications (insulin, pain medication, antibiotics), this could lead to:
   - Disease progression
   - Uncontrolled pain
   - Medical complications
   - Death (in severe cases)

2. **Overdosing scenario**: If a different overflow pattern occurs, the patient could receive a massive overdose. This could lead to:
   - Toxicity
   - Organ damage
   - Death

## Answer Key

1. **Which function contains the vulnerability?**
   - `calculate_volume_to_deliver`

2. **What specific line(s) of code is vulnerable?**
   - Line 77: `uint32_t volume_microliters = pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;`

3. **What values of `rate` and `duration_ms` would you use to exploit this vulnerability?**
   - Rate: 1000 ml/hr
   - Duration: 4294968 ms

4. **How many milliliters would actually be delivered with your exploit values vs. how many are expected?**
   - Expected: ~1193 ml
   - Actually delivered: ~0 ml (may vary slightly depending on exact overflow)

5. **In a real-world scenario, what could be the potential impact of this vulnerability to patient safety?**
   - Patients could receive virtually no medication when they should be receiving critical medications, leading to untreated conditions, medical complications, or death
   - In other overflow scenarios, patients could receive massive overdoses, leading to toxicity and death

## Flag

Using the exploit values:

`flag{1000}_{4294968}_{0}`

## Mitigation

To fix this vulnerability:

1. Use large enough data types to prevent overflow:
```c
uint64_t volume_microliters = (uint64_t)pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;
```

2. Add bounds checking before performing the calculation:
```c
if ((uint64_t)pump_state.rate_ml_per_hour * time_increment_ms * 1000 > UINT32_MAX) {
    // Handle overflow condition
    report_error();
    return safe_fallback_value;
}
```

3. Implement redundant safety checks to validate calculated values against expected ranges.