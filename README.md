# SPQR Medical Infusion Pump Challenge

Welcome to the SPQR CTF Challenge! Your task is to analyze a vulnerable infusion pump firmware.

## Background

You've been hired as a security consultant to audit the firmware of a new infusion pump before it goes to market. Your goal is to identify security vulnerabilities that could affect patient safety.

## The Challenge

The provided code is a simplified version of an infusion pump controller that dispenses medication. Your mission is to find a critical vulnerability that could cause the pump to deliver an incorrect dose of medication.

## Getting Started

### Web Version (Recommended)
Access the challenge at: https://spqr.rice.edu/infusion-pump-ctf/

The web interface simulates the pump with the same vulnerabilities as the C version.

### Command Line Version (Optional)
If you prefer working with C code directly:

```bash
gcc -lm -o infusion_pump infusionvuln.c
./infusion_pump
```

## Challenge Objective

1. Find the security vulnerability
2. Determine the exact input values that trigger it
3. Understand how this could lead to patient harm
4. Submit your findings through the Google Form

## Submission

When you've found the vulnerability, submit your answers through the Google Form link provided separately. Do not post your solutions publicly.

The form will ask for:
- The specific vulnerability you found
- The input values that trigger it
- The expected vs. actual medication delivered
- Real-world impact assessment

## Good luck!

Remember that in medical device security, the stakes are high - your work could literally save lives.