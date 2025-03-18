# SPQR Infusion Pump CTF - Walkthrough Guide

This guide provides a step-by-step approach to solving the SPQR Infusion Pump CTF challenge. If you're stuck, use this for hints without immediately jumping to the full solution.

## Step 1: Understanding the Code

Start by understanding what the code does:

1. The program simulates an infusion pump that delivers medication at a specified rate
2. Look through each function to understand its purpose
3. Pay special attention to the `calculate_volume_to_deliver()` function and how it calculates the medication volume

## Step 2: Identifying Vulnerable Areas

Focus on these areas:

1. What data types are used for various calculations?
2. Where might calculations exceed the maximum value of their data type?
3. Are there any administrative or bypass functions that could be misused?

## Step 3: Understanding Integer Overflow

The key vulnerability involves integer overflow:

1. When arithmetic operations exceed the maximum value a data type can hold, the result "wraps around" to a smaller value
2. For example, if a `uint32_t` (max value: 4,294,967,295) exceeds its maximum, it will wrap around to 0 and continue

## Step 4: Finding the Vulnerability

Look at `calculate_volume_to_deliver()` closely:

```c
uint32_t volume_microliters = pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;
```

Think about:
1. What happens if `pump_state.rate_ml_per_hour * time_increment_ms * 1000` exceeds 2^32?
2. How would that affect the calculated volume?

## Step 5: Crafting an Exploit

To create your exploit:

1. Use `admin_set_custom_parameters()` to set custom values
2. Choose a high value for `rate` (try the maximum allowed: 1000 ml/hr)
3. Calculate what `duration_ms` would cause overflow: find a value where `rate * duration_ms * 1000 > 2^32`
4. Try your exploit and observe the results - what volume is actually delivered vs. what was expected?

## Step 6: Refining Your Answer

For the most effective exploit:

1. Choose values that maximize the difference between expected and actual volumes
2. Calculate the expected volume using a 64-bit integer (to avoid overflow)
3. Compare with the actual volume delivered by the vulnerable pump

## Step 7: Understanding the Impact

Consider real-world implications:

1. What medical scenarios would be affected by this vulnerability?
2. Could this lead to under-medication? Over-medication?
3. What would happen if the patient is receiving critical medication?

Remember: The flag format is `flag{rate_value}_{duration_ms_value}_{actual_volume_delivered}`

Good luck!