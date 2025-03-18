# SPQR Infusion Pump CTF - Solution Guide

This document provides the detailed solution for the SPQR Infusion Pump CTF challenge.

## Vulnerability Overview

The infusion pump firmware contains an **integer overflow vulnerability** in the `calculate_volume_to_deliver` function. This vulnerability allows an attacker to cause the pump to deliver a significantly lower amount of medication than intended, which could be dangerous or fatal in a real medical device.

## Technical Analysis

### Vulnerable Function

The vulnerability is in the `calculate_volume_to_deliver()` function:

```c
// Calculate how much fluid should be delivered based on elapsed time
double calculate_volume_to_deliver(uint32_t time_increment_ms) {
    // Vulnerable calculation - integer overflow with large values
    uint32_t volume_microliters = pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;
    
    // Convert back to milliliters with decimal precision
    double volume_ml = (double)volume_microliters / 1000.0;
    
    return volume_ml;
}
```

### The Vulnerability

The critical vulnerability is in this line:

```c
uint32_t volume_microliters = pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;
```

This calculation is vulnerable to integer overflow because:

1. `pump_state.rate_ml_per_hour` is a `uint16_t` (max value: 65,535)
2. `time_increment_ms` is a `uint32_t` (max value: 4,294,967,295)
3. The multiplication `rate * time * 1000` can easily exceed the maximum value of a `uint32_t` (4,294,967,295)

When integer overflow occurs, the result "wraps around" to a much smaller value, causing the calculation to return a significantly lower volume than intended.

### Exploitation Path

The vulnerability can only be exploited using admin access, as the normal user interface has safety limits that prevent using values large enough to trigger the overflow.

## Exploitation Steps

1. First, access the admin panel:
   - Find the hard-coded password in the source code: `SPQR2025`
   - In the web interface: Click "Admin Access" and enter the password
   - In the command-line: Select option 6 for Admin Access and enter the password

2. Trigger the vulnerability:
   - Set `rate = 2500` ml/hr (a high but plausible medical value)
   - Set `duration = 3000000` milliseconds (50 minutes, again plausible)

3. Start the infusion:
   - In the web interface: Click "Start Infusion"
   - In the command-line: Select option 4 to start the infusion

4. Observe the results:
   - The calculation `2500 * 3000000 * 1000 = 7,500,000,000,000` exceeds 2^32
   - Expected delivery (correct math): 2083.33 ml
   - Actual delivery: ~465.66 ml (due to integer overflow)
   - This represents a severe underdose of approximately 77%

## Real-World Impact

In a real medical scenario:

1. **Critical Medication Underdosing**: 
   - For medications like antibiotics, chemotherapy, or critical care drugs, receiving only 22% of the prescribed dose could be life-threatening
   - Patient's condition would deteriorate despite the pump appearing to function correctly
   - The underdosing would be difficult to detect without independent verification

2. **Medical Consequences**:
   - Infections would progress despite antibiotic therapy
   - Pain would remain uncontrolled
   - Critical conditions would deteriorate
   - In severe cases, this could lead to death

## Solution for CTF Questions

### Easy Questions
1. **What medical device is being simulated?** Infusion pump
2. **What programming language is used?** C (and JavaScript for web version)
3. **What special access is required?** Admin access
4. **What is the admin password?** SPQR2025

### Medium Questions
1. **Default infusion rate?** 100 ml/hr
2. **Maximum normal rate?** 999 ml/hr or 1000 ml/hr
3. **Data type for infusion rate?** uint16_t
4. **Function calculating delivery volume?** calculate_volume_to_deliver()

### Hard Questions
1. **Vulnerability type?** Integer overflow
2. **Vulnerable calculation?** `rate * time * 1000` in calculate_volume_to_deliver()
3. **Exploit values:** rate = 2500 ml/hr, duration = 3000000 ms (50 minutes)
4. **Impact?** Severe underdosing (only ~22% of intended medication delivered)

### Expert Question
1. **Optimal exploit values:** rate = 2500, duration = 3000000
2. **Expected volume:** 2083.33 ml
3. **Actual volume:** 465.66 ml
4. **Discrepancy:** 1617.67 ml (77.7% less medication)

## Mitigation

To fix this vulnerability:

1. Use larger data types to prevent overflow:
```c
uint64_t volume_microliters = (uint64_t)pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;
```

2. Add bounds checking:
```c
if ((uint64_t)pump_state.rate_ml_per_hour * time_increment_ms * 1000 > UINT32_MAX) {
    // Handle overflow condition
    report_error();
    return safe_fallback_value;
}
```

3. Implement redundant safety checks for critical calculations.