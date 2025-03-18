# Infusion Pump CTF - Web Version

This is the JavaScript implementation of the Infusion Pump CTF challenge, allowing participants to exploit the vulnerability directly from their web browser.

## Setup Instructions

### Running the Application

1. Start a local web server:

```bash
# Use the provided script:
./serve.sh

# Or manually start a server:
# If you have Python 3:
cd web
python3 -m http.server

# If you have Python 2:
cd web
python -m SimpleHTTPServer
```

2. Open your web browser and navigate to:

```
http://localhost:8000
```

## Using the Simulator

The web interface provides a full simulation of an infusion pump with the following features:

1. **Basic Pump Controls**:
   - Set patient information
   - Configure infusion rate and volume
   - Start and stop infusion
   - Monitor status in real-time

2. **Admin Functions** (password protected):
   - Access by clicking "Admin Access"
   - Enter the admin password (hint: look for it in the JavaScript code)
   - Set custom parameters (bypassing normal safety checks)
   - Run full simulations with precise control over duration
   - Execute safety checks

## CTF Challenge

The goal of this CTF challenge is to:

1. Find the admin password by examining the JavaScript source code
2. Identify the integer overflow vulnerability in the calculation function
3. Determine input values that will exploit this vulnerability
4. Observe the difference between expected and actual medication delivery
5. Submit your findings according to the CTF platform's requirements

The vulnerability causes the pump to deliver an incorrect amount of medication due to integer overflow when calculating the volume to deliver.

## Implementation Details

- The JavaScript implementation simulates the same integer overflow vulnerability that exists in the C version
- The web interface provides an intuitive way to interact with the vulnerable code
- Console output shows the results of calculations and operations
- Debug messages in the browser console show expected vs. actual delivery volumes

## Exploit Hints

- The vulnerable code is in the `calculateVolumeToDeliver()` function
- Pay attention to the values used in the calculation and their limits
- Try setting extreme values in the admin panel and observe the results
- When you find a combination that causes a large discrepancy between expected and actual volumes, you've found the exploit!

Good luck with the challenge!