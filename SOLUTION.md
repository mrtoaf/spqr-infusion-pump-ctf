# SPQR Infusion Pump CTF - Solution Guide

This document provides the detailed solution for the SPQR Infusion Pump CTF challenge, along with a methodical approach to discovering the vulnerability.

## Vulnerability Overview

The infusion pump firmware contains an **integer overflow vulnerability** in the `calculate_volume_to_deliver` function. This vulnerability allows an attacker to cause the pump to deliver a significantly lower amount of medication than intended, which could be dangerous or fatal in a real medical device.

## Methodical Approach to Discovery

### Step 1: Understanding the Code
Begin by examining the codebase to understand the core functionality:
- The pump calculates medication delivery based on flow rate and time
- Critical calculations happen in the `calculate_volume_to_deliver()` function
- Admin mode allows bypassing normal safety limits

### Step 2: Accessing Admin Mode
Regular user interfaces have safety limits that prevent triggering the vulnerability:
1. Find the admin password in the source code: `SPQR2025`
2. In the web interface, click "Admin Access" and enter the password
3. In the command-line version, select option 6 for Admin Access
4. Admin mode allows setting parameters beyond the normal safety limits

### Step 3: Identifying the Vulnerability Type
Analyze the code's data types and operations:
1. Notice that `pump_state.rate_ml_per_hour` is a `uint16_t` (max value: 65,535)
2. `time_increment_ms` is a `uint32_t` (max value: 4,294,967,295)
3. The multiplication operations could potentially exceed data type limits

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

1. The multiplication `rate * time * 1000` can easily exceed the maximum value of a `uint32_t` (4,294,967,295)
2. When integer overflow occurs, the result "wraps around" to a much smaller value
3. This causes the calculation to return a significantly lower volume than intended

## Exploitation Steps

1. Access the admin panel using the password `SPQR2025`

2. Trigger the vulnerability:
   - Set `rate = 2500` ml/hr (a high but plausible medical value)
   - Set `duration = 3000000` milliseconds (50 minutes, also plausible)

3. Start the infusion and observe the results:
   - The calculation `2500 * 3000000 * 1000 = 7,500,000,000,000` exceeds 2^32
   - Expected delivery (correct math): 2083.33 ml
   - Actual delivery: ~465.66 ml (due to integer overflow)
   - This represents a severe underdose of approximately 77%

## Real-World Impact

In a medical scenario, this vulnerability could have serious consequences:

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

### Programming Questions
1. **What programming language is used?** C (and JavaScript for web version)
2. **What data type is used to store the infusion rate?** uint16_t
3. **Which function calculates dosages?** calculate_volume_to_deliver()

### Vulnerability Questions
1. **What specific lines of code are vulnerable?** Line with `uint32_t volume_microliters = pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;`
2. **What input values would trigger the vulnerability?** rate = 2500 ml/hr, duration = 3000000 ms (50 minutes)
3. **How could this harm patients?** Severe underdosing of critical medications, potentially leading to treatment failure or death

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

3. Implement redundant safety checks for critical calculations
4. Use floating-point calculations for the entire operation to avoid integer overflow