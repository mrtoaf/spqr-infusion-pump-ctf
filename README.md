# SPQR CTF Challenge
## Infusion Pump Integer Overflow

Welcome to the SPQR CTF Challenge! You're about to analyze a simplified infusion pump firmware for vulnerabilities.

### Background Story

You've been hired as a security consultant to audit the firmware of a new infusion pump before it goes to market. The development team is particularly concerned about potential issues in the dosage calculation functions.

### Challenge Description

The provided code is a simplified version of an infusion pump controller. Your task is to:

1. Identify a vulnerability in the code
2. Determine input values that would trigger this vulnerability
3. Explain how this vulnerability could lead to patient harm in a real-world scenario
4. Submit the specific input values that maximize the impact of the exploit

### Source Code Access

In the web version, you can view a decompiled version of the firmware by clicking the "View Decompiled Source" button. This represents what a security researcher might obtain after extracting and reverse-engineering the firmware binary from a real medical device. In actual security research scenarios, manufacturers rarely provide source code, requiring researchers to extract binaries from device memory and decompile them for analysis.

### Objective

Cause the pump to deliver a significantly incorrect amount of medication!

### Getting Started

#### Command Line Version
To compile and run the command line version:

```bash
gcc -Wall -o infusion_pump infusionvuln.c
./infusion_pump
```

When the program starts, you'll see a menu-driven interface. This simulates the interface of the infusion pump control system.

#### Web Version
The challenge is also available as a web application that simulates the vulnerable code in JavaScript:

1. Run the web server:
```bash
./serve.sh
```

2. Open http://localhost:8000 in your browser

### Admin Access (Required for the CTF)

#### In Command Line Version:
1. Select option 7 (Admin Access)
2. Enter the admin password (hint: look for it in the source code)
3. Once authenticated, you'll have access to additional functions that bypass normal safety checks

#### In Web Version:
1. Click the "Admin Access" button
2. Enter the admin password (hint: look in the JavaScript source code)
3. Use the admin panel to set custom parameters and run simulations

### Questions to Answer (Submit through the CTF platform)

1. Which function contains the vulnerability?
2. What specific line(s) of code is vulnerable?
3. What values of `rate` and `duration_ms` would you use to exploit this vulnerability?
4. How many milliliters would actually be delivered with your exploit values vs. how many are expected?
5. In a real-world scenario, what could be the potential impact of this vulnerability to patient safety?

### Hints

- Pay close attention to variable data types and their maximum values
- Consider what happens in calculations when a value exceeds the maximum size of its data type
- Remember that some vulnerabilities don't usually cause a crash or error - it silently wraps around to produce incorrect results
- In the web version, check the browser's developer console (F12) to see debug messages

### Flag Format

Once you've identified the vulnerability and determined the exploitable input values, submit your answer in the following format:

`flag{rate_value}_{duration_ms_value}_{actual_volume_delivered}`

For example: `flag{100}_{36000000}_{1000}`

Good luck, and remember: in medical device security, your work could literally save lives!