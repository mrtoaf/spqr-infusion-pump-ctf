# SPQR Infusion Pump CTF - Walkthrough Guide

This guide provides a step-by-step approach to solving the SPQR Infusion Pump CTF challenge. If you're stuck, use this for hints without immediately jumping to the full solution.

## Step 1: Understanding the Code

Start by examining both versions of the code:

1. Use the "View Decompiled Source" button in the web interface
2. Alternatively, examine the `infusionvuln.c` file directly
3. Understand how the infusion pump calculates and delivers medication
4. Pay special attention to the `calculate_volume_to_deliver()` function

## Step 2: Accessing Admin Mode

Regular user interfaces have safety limits that prevent triggering the vulnerability:

1. Find the admin password by examining the source code
2. In the web interface, click "Admin Access" and enter the password
3. In the command-line version, select option 6 for Admin Access
4. Admin mode allows setting parameters beyond the normal safety limits

## Step 3: Identifying the Vulnerability Type

Analyze the code's data types and operations:

1. What numeric data types are used in critical calculations?
2. Are there any operations that could exceed the limits of these data types?
3. What happens when arithmetic operations exceed the maximum value a data type can hold?

## Step 4: Locating the Vulnerable Code

Look at the calculation in `calculate_volume_to_deliver()`:

```c
// Vulnerable calculation - integer overflow with large values
uint32_t volume_microliters = pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;
```

Think about:
1. What is the maximum value of a `uint32_t`?
2. What input values would cause this calculation to exceed that maximum?
3. How would overflow affect the delivered medication volume?

## Step 5: Crafting Your Exploit

Develop a strategy to exploit the vulnerability:

1. Choose a high rate value (try values around 1000-2500 ml/hr)
2. Calculate a duration that would cause the intermediate calculation to overflow
3. Remember that `rate * duration * 1000` must exceed 2^32 (4,294,967,296)
4. Use the admin panel to test these values

## Step 6: Analyzing the Results

Compare the expected vs. actual results:

1. Calculate the mathematically expected volume delivery (using 64-bit math)
2. Observe what the pump actually delivers (simulated in the user interface)
3. Quantify the discrepancy between expected and actual delivery
4. Determine if the vulnerability leads to underdosing or overdosing

## Step 7: Understanding Real-World Impact

Consider the medical implications:

1. How would this vulnerability affect a real patient?
2. What types of medications would be most dangerous if underdosed?
3. What safety features should be implemented to prevent such vulnerabilities?

## Submit Your Findings

When you've fully analyzed the vulnerability:

1. Document the specific vulnerability type
2. Record the exact values that trigger the exploit
3. Calculate the expected vs. actual medication delivery
4. Submit your answers via the Google Form

Good luck with your security analysis!