// Main JavaScript interface for the Infusion Pump simulator
// This is a pure JavaScript implementation that simulates the vulnerable infusion pump

// DOM elements
const consoleOutput = document.getElementById('console');
const patientIdInput = document.getElementById('patient-id');
const medicationInput = document.getElementById('medication');
const rateInput = document.getElementById('rate');
const volumeInput = document.getElementById('volume');
const durationInput = document.getElementById('duration');
const statusValue = document.getElementById('status-value');
const elapsedTimeDisplay = document.getElementById('elapsed-time');
const volumeInfusedDisplay = document.getElementById('volume-infused');
const startButton = document.getElementById('start-infusion');
const stopButton = document.getElementById('stop-infusion');
const adminRateInput = document.getElementById('admin-rate');
const adminVolumeInput = document.getElementById('admin-volume');
const adminDurationInput = document.getElementById('admin-duration');
const adminModal = document.getElementById('admin-modal');
const adminPanel = document.getElementById('admin-panel');
const overlay = document.getElementById('overlay');
const loadingScreen = document.getElementById('loading-screen');

// Global variables
let pumpFunctions = {}; // Will hold pump function implementations
let isModuleReady = false;
let simulationTimer = null;
const SIMULATION_INTERVAL = 500; // 0.5 seconds (show more frequent updates)

// Initialize once the page loads
document.addEventListener('DOMContentLoaded', function() {
    setTimeout(function() {
        try {
            initializePumpFunctions();
            isModuleReady = true;
            hideLoadingScreen();
            log('Infusion Pump Firmware loaded successfully', 'success');
            setupEventListeners();
            
            // Show the tutorial modal after a short delay
            setTimeout(function() {
                showTutorial();
            }, 300);
        } catch (error) {
            console.error("Error during initialization:", error);
            log('Error during initialization: ' + error.message, 'error');
            hideLoadingScreen();
        }
    }, 500); // Short delay to simulate loading
});

// Initialize the pump functions
function initializePumpFunctions() {
    console.log("Initializing pump functions");
    
    // State variables for the pump
    let pumpState = {
        patient_id: "UNKNOWN",
        medication: "SALINE",
        rate_ml_per_hour: 100,
        volume_to_infuse_ml: 500,
        duration_ms: 18000000, // Default duration for 500ml at 100ml/hr
        volume_infused_ml: 0,
        is_running: false,
        elapsed_time_ms: 0,
        from_admin_panel: false  // Flag to track if parameters were set via admin panel
    };
    
    // Create pump functions
    pumpFunctions = {
        // Core pump functions
        initPump: function() { 
            console.log("init_pump called");
            pumpState.patient_id = "UNKNOWN";
            pumpState.medication = "SALINE";
            pumpState.rate_ml_per_hour = 100;
            pumpState.volume_to_infuse_ml = 500;
            pumpState.duration_seconds = 5000; // Default duration in seconds
            pumpState.duration_ms = pumpState.duration_seconds * 1000; // Convert to ms for internal use
            pumpState.volume_infused_ml = 0;
            pumpState.is_running = false;
            pumpState.elapsed_time_ms = 0;
            pumpState.from_admin_panel = false; // Reset admin flag on init
            return 0;
        },
        setInfusionRate: function(rate) { 
            console.log("set_infusion_rate called with rate:", rate);
            
            // For regular interface, enforce a stricter limit of 999 ml/hr
            if (rate > 999) {
                log("ERROR: Rate exceeds maximum allowed (999 ml/hr)", "error");
                rateInput.value = 999; // Reset to max allowed
                return -1;
            }
            
            pumpState.rate_ml_per_hour = rate;
            pumpState.from_admin_panel = false; // Reset admin flag when using regular interface
            return 0;
        },
        setVolumeToInfuse: function(vol) { 
            console.log("set_volume_to_infuse called with volume:", vol);
            pumpState.volume_to_infuse_ml = vol;
            pumpState.from_admin_panel = false; // Reset admin flag when using regular interface
            
            // Recalculate duration based on new volume (if rate > 0)
            if (pumpState.rate_ml_per_hour > 0) {
                // duration(ms) = volume(ml) / rate(ml/hr) * 3600000
                const newDurationMs = Math.floor((vol / pumpState.rate_ml_per_hour) * 3600000);
                pumpState.duration_ms = newDurationMs;
                
                // Convert to seconds for display
                pumpState.duration_seconds = Math.floor(newDurationMs / 1000);
                durationInput.value = pumpState.duration_seconds;
            }
            return 0;
        },
        setDuration: function(durationSeconds) {
            console.log("set_duration called with duration (seconds):", durationSeconds);
            
            // For regular interface, enforce a limit of 3600 seconds (1 hour)
            if (durationSeconds > 3600) {
                log("ERROR: Duration exceeds maximum allowed (3,600 seconds / 1 hour)", "error");
                durationInput.value = 3600; // Reset to max allowed
                durationSeconds = 3600;
            }
            
            // Store duration in seconds for display
            pumpState.duration_seconds = durationSeconds;
            
            // Convert to milliseconds for internal calculations
            const durationMs = durationSeconds * 1000;
            pumpState.duration_ms = durationMs;
            
            pumpState.from_admin_panel = false; // Reset admin flag when using regular interface
            
            // Recalculate volume based on new duration (if rate > 0)
            if (pumpState.rate_ml_per_hour > 0) {
                // volume(ml) = rate(ml/hr) * duration(ms) / 3600000
                pumpState.volume_to_infuse_ml = Math.floor((pumpState.rate_ml_per_hour * durationMs) / 3600000);
                volumeInput.value = pumpState.volume_to_infuse_ml;
            }
            return 0;
        },
        // Always use the same vulnerable calculation for both admin and regular modes
        // The vulnerability is protected by UI input limits for regular users
        calculateVolumeToDeliver: function(time) {
            console.log("calculate_volume_to_deliver called with time:", time);
            
            // Always use the vulnerable calculation regardless of mode
            // This is vulnerable to integer overflow with large values
            
            // This simulates the vulnerable calculation with integer overflow
            const MAX_UINT32 = 4294967295;
            
            // Single calculation that can overflow with large values
            let intermediate = pumpState.rate_ml_per_hour * time * 1000;
            
            // Apply 32-bit overflow if necessary
            if (intermediate > MAX_UINT32) {
                // Realistic integer overflow - the value wraps around to a much smaller value
                intermediate = intermediate % (MAX_UINT32 + 1);
                console.log("OVERFLOW DETECTED - value wrapped around to:", intermediate);
            }
            
            // Complete the calculation by dividing
            let volume_microliters = intermediate / 3600000;
            
            // For diagnostic purposes (when running with large values)
            if (pumpState.from_admin_panel === true && time > 1000000) {
                // Calculate the expected value using 64-bit math
                const expected_microliters = Number(BigInt(pumpState.rate_ml_per_hour) * BigInt(time) * BigInt(1000) / BigInt(3600000));
                const expected_ml = expected_microliters / 1000;
                console.log("Expected correct delivery:", expected_ml.toFixed(2), "ml");
                console.log("Actual delivery with overflow:", (volume_microliters/1000).toFixed(2), "ml (UNDERDOSE)");
            }
            
            // Convert back to milliliters (keep decimal precision)
            return volume_microliters / 1000;
        },
        runInfusionCycle: function(time) {
            console.log("run_infusion_cycle called with time:", time);
            
            if (!pumpState.is_running) return;
            
            // Update elapsed time
            pumpState.elapsed_time_ms += time;
            
            // Calculate volume to deliver during this cycle
            let volumeToDeliver;
            
            // Simply use calculateVolumeToDeliver which now handles both regular and admin modes
            volumeToDeliver = this.calculateVolumeToDeliver(time);
            
            // Add diagnostic messages only for admin mode with large parameters
            if (pumpState.from_admin_panel === true && 
                (pumpState.rate_ml_per_hour >= 1000 || pumpState.duration_seconds >= 4000)) {
                console.log("Admin mode with large values - potentially vulnerable to overflow");
                
                // Only show the warning exactly once at the beginning of the infusion
                if (pumpState.elapsed_time_ms === SIMULATION_INTERVAL) {
                    // Calculate what SHOULD be delivered with proper 64-bit math
                    const expected_microliters = Number(BigInt(pumpState.rate_ml_per_hour) * BigInt(pumpState.duration_ms) * BigInt(1000) / BigInt(3600000));
                    const expected_ml = Math.floor(expected_microliters / 1000);
                    
                    // Calculate what will ACTUALLY be delivered with 32-bit overflow
                    const actual_ml = this.calculateVolumeToDeliver(pumpState.duration_ms);
                    
                    // Check for significant discrepancy indicating underdose
                    if (expected_ml > actual_ml && (expected_ml - actual_ml) > 100) {
                        log("---------------------------------------------", "normal");
                        log("System notice: Calculation anomaly detected", "warning");
                        log("Expected volume: " + expected_ml.toFixed(2) + " ml", "warning");
                        log("Actual delivery: " + actual_ml.toFixed(2) + " ml", "warning");
                        log("MEDICATION UNDERDOSE POSSIBLE - Please contact system administrator", "warning");
                    }
                }
            } else {
                // Use the safe calculation in normal mode
                volumeToDeliver = this.calculateVolumeToDeliver(time);
            }
            
            // Update the infused volume
            pumpState.volume_infused_ml += volumeToDeliver;
            
            // Check if infusion is complete
            if (pumpState.volume_infused_ml >= pumpState.volume_to_infuse_ml) {
                pumpState.is_running = false;
                log("Infusion complete! Delivered " + pumpState.volume_infused_ml + " ml");
            }
        },
        startInfusion: function() {
            console.log("start_infusion called");
            
            if (pumpState.is_running) {
                log("Infusion already in progress");
                return;
            }
            
            // Check if we're using admin parameters (which might trigger the vulnerability)
            // Only allow admin mode if the from_admin_panel flag is set
            const isAdminMode = (pumpState.rate_ml_per_hour >= 1000 || pumpState.duration_seconds >= 4000) && 
                               pumpState.from_admin_panel === true;
            
            // If in admin mode, recalculate the volume based on the custom parameters
            if (isAdminMode) {
                // Pre-calculate the expected volume for the full duration using the vulnerable calculation
                const calcVolume = this.calculateVolumeToDeliver(pumpState.duration_ms);
                console.log("Using admin parameters - volume calculated: " + calcVolume);
                
                // Set the volume to infuse based on the calculated volume
                pumpState.volume_to_infuse_ml = calcVolume;
                
                log("ADMIN MODE: Using custom parameters");
            }
            
            // Log the infusion details (only once)
            log("Starting infusion...");
            log("Patient ID: " + pumpState.patient_id);
            log("Medication: " + pumpState.medication);
            log("Rate: " + pumpState.rate_ml_per_hour + " ml/hr");
            log("Volume to infuse: " + pumpState.volume_to_infuse_ml + " ml");
            log("Duration: " + pumpState.duration_seconds + " seconds");
            
            pumpState.is_running = true;
        },
        stopInfusion: function() {
            console.log("stop_infusion called");
            
            if (pumpState.is_running) {
                pumpState.is_running = false;
                log("Infusion stopped");
            }
        },
        
        // Admin functions
        validateAdminPassword: function(pw) { 
            console.log("validate_admin_password called with password:", pw);
            // Hard-coded password - intentional vulnerability
            return pw === "SPQR2025" ? 1 : 0;
        },
        adminSetCustomParameters: function(rate, volume, durationSeconds) { 
            console.log("admin_set_custom_parameters called with rate:", rate, "volume:", volume, "duration (seconds):", durationSeconds);
            
            // Update pump state (bypassing normal checks)
            pumpState.rate_ml_per_hour = rate;
            pumpState.volume_to_infuse_ml = volume;
            pumpState.duration_seconds = durationSeconds;
            pumpState.duration_ms = durationSeconds * 1000; // Convert to ms for internal calculations
            pumpState.from_admin_panel = true; // Set the admin flag to enable vulnerability
            
            // Update the main interface inputs
            rateInput.value = rate;
            volumeInput.value = volume;
            durationInput.value = durationSeconds;
            
            // Simple log message without revealing vulnerability
            log("ADMIN: Custom parameters have been set", "normal");
        },
        
        // Getters and setters
        getRate: function() { 
            return pumpState.rate_ml_per_hour; 
        },
        getVolumeToInfuse: function() { 
            return pumpState.volume_to_infuse_ml; 
        },
        getVolumeInfused: function() { 
            return pumpState.volume_infused_ml; 
        },
        isPumpRunning: function() { 
            return pumpState.is_running ? 1 : 0; 
        },
        getElapsedTime: function() { 
            return pumpState.elapsed_time_ms; 
        },
        getDuration: function() {
            return pumpState.duration_seconds;
        },
        getDurationMs: function() {
            return pumpState.duration_ms;
        },
        getPatientId: function() { 
            return pumpState.patient_id; 
        },
        getMedication: function() { 
            return pumpState.medication; 
        },
        setPatientId: function(id) {
            console.log("set_patient_id called with id:", id);
            pumpState.patient_id = id;
        },
        setMedication: function(med) {
            console.log("set_medication called with medication:", med);
            pumpState.medication = med;
        }
    };
    
    console.log("Pump functions initialized successfully");
    pumpFunctions.initPump();
    updateDisplay();
}

// Hide loading screen
function hideLoadingScreen() {
    loadingScreen.style.display = 'none';
}

// Console logging function
function log(message, type = 'normal') {
    const logEntry = document.createElement('div');
    logEntry.className = type;
    logEntry.textContent = message;
    consoleOutput.appendChild(logEntry);
    consoleOutput.scrollTop = consoleOutput.scrollHeight;
}

// Clear the console
function clearConsole() {
    consoleOutput.innerHTML = '';
}

// Format time from milliseconds to HH:MM:SS
function formatTime(milliseconds) {
    const totalSeconds = Math.floor(milliseconds / 1000);
    const hours = Math.floor(totalSeconds / 3600);
    const minutes = Math.floor((totalSeconds % 3600) / 60);
    const seconds = totalSeconds % 60;
    
    return `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
}

// Update the display with current pump state
function updateDisplay() {
    if (!isModuleReady) return;
    
    const isRunning = pumpFunctions.isPumpRunning();
    const elapsedTime = pumpFunctions.getElapsedTime();
    const volumeInfused = pumpFunctions.getVolumeInfused();
    
    // Format volume infused with 2 decimal places to show small changes
    const formattedVolume = volumeInfused.toFixed(2);
    
    statusValue.textContent = isRunning ? 'Running' : 'Stopped';
    elapsedTimeDisplay.textContent = formatTime(elapsedTime);
    volumeInfusedDisplay.textContent = `${formattedVolume} ml`;
    
    // Update input fields to match current state
    patientIdInput.value = pumpFunctions.getPatientId();
    medicationInput.value = pumpFunctions.getMedication();
    rateInput.value = pumpFunctions.getRate();
    volumeInput.value = pumpFunctions.getVolumeToInfuse();
    durationInput.value = pumpFunctions.getDuration();
    
    // Update button states
    startButton.disabled = isRunning;
    stopButton.disabled = !isRunning;
}

// Modal functions
function showModal(modal) {
    modal.style.display = 'block';
    overlay.style.display = 'block';
}

function hideModal(modal) {
    modal.style.display = 'none';
    overlay.style.display = 'none';
}

// Core pump operations
function resetPump() {
    if (!isModuleReady) return;
    
    stopSimulation();
    pumpFunctions.initPump();
    log("Pump has been reset to default settings", "normal");
    updateDisplay();
}

function setInfusionRate() {
    if (!isModuleReady) return;
    
    // Get and validate the rate value
    let newRate = parseInt(rateInput.value, 10);
    
    // Additional client-side validation to ensure we stay within safe limits
    if (newRate > 999) {
        newRate = 999;
        rateInput.value = 999;
        log("WARNING: Rate limited to maximum safe value (999 ml/hr)", "warning");
    }
    
    pumpFunctions.setInfusionRate(newRate);
    
    // Recalculate duration based on volume and new rate
    if (newRate > 0) {
        const currentVolume = pumpFunctions.getVolumeToInfuse();
        // duration(ms) = volume(ml) / rate(ml/hr) * 3600000
        const newDuration = Math.floor((currentVolume / newRate) * 3600000);
        pumpFunctions.setDuration(newDuration);
    }
    
    updateDisplay();
}

function setVolumeToInfuse() {
    if (!isModuleReady) return;
    
    const newVolume = parseInt(volumeInput.value, 10);
    pumpFunctions.setVolumeToInfuse(newVolume);
    updateDisplay();
}

function setDuration() {
    if (!isModuleReady) return;
    
    // Get and validate the duration value (in seconds)
    let newDurationSeconds = parseInt(durationInput.value, 10);
    
    // Additional client-side validation to ensure we stay within safe limits
    if (newDurationSeconds > 3600) {
        newDurationSeconds = 3600;
        durationInput.value = 3600;
        log("WARNING: Duration limited to maximum safe value (3,600 seconds / 1 hour)", "warning");
    }
    
    pumpFunctions.setDuration(newDurationSeconds);
    updateDisplay();
}

function startSimulation() {
    if (!isModuleReady) return;
    
    // Update patient info
    pumpFunctions.setPatientId(patientIdInput.value);
    pumpFunctions.setMedication(medicationInput.value);
    
    // Start infusion (logs will be handled by the startInfusion function)
    pumpFunctions.startInfusion();
    updateDisplay();
    
    // Begin simulation timer
    simulationTimer = setInterval(() => {
        pumpFunctions.runInfusionCycle(SIMULATION_INTERVAL);
        updateDisplay();
        
        // If pump is no longer running, stop the timer
        if (!pumpFunctions.isPumpRunning()) {
            stopSimulation();
        }
    }, SIMULATION_INTERVAL);
}

function stopSimulation() {
    if (simulationTimer) {
        clearInterval(simulationTimer);
        simulationTimer = null;
    }
    
    if (isModuleReady) {
        pumpFunctions.stopInfusion();
        updateDisplay();
    }
}

// Admin functions
function validateAdminPassword() {
    if (!isModuleReady) return false;
    
    const password = document.getElementById('admin-password').value;
    const errorMsg = document.getElementById('password-error');
    
    if (pumpFunctions.validateAdminPassword(password)) {
        hideModal(adminModal);
        showModal(adminPanel);
        errorMsg.textContent = '';
        document.getElementById('admin-password').value = '';
        return true;
    } else {
        errorMsg.textContent = 'Invalid password. Try again.';
        return false;
    }
}

function setCustomParameters() {
    if (!isModuleReady) return;
    
    const rate = parseInt(adminRateInput.value, 10);
    const volume = parseInt(adminVolumeInput.value, 10);
    const durationSeconds = parseInt(adminDurationInput.value, 10);
    pumpFunctions.adminSetCustomParameters(rate, volume, durationSeconds);
}

// Function removed - we now use the regular Start Infusion button instead

// Show tutorial modal
function showTutorial() {
    const tutorialModal = document.getElementById('tutorial-modal');
    tutorialModal.style.display = 'block';
    overlay.style.display = 'block';
}

// Hide tutorial modal
function hideTutorial() {
    const tutorialModal = document.getElementById('tutorial-modal');
    tutorialModal.style.display = 'none';
    overlay.style.display = 'none';
}

// Show source code modal
function showSourceCode() {
    const sourceModal = document.getElementById('source-modal');
    sourceModal.style.display = 'block';
    overlay.style.display = 'block';
}

// Hide source code modal
function hideSourceCode() {
    const sourceModal = document.getElementById('source-modal');
    sourceModal.style.display = 'none';
    overlay.style.display = 'none';
}

// Show GDB tutorial modal
function showGdbTutorial() {
    const gdbTutorialModal = document.getElementById('gdb-tutorial-modal');
    gdbTutorialModal.style.display = 'block';
    overlay.style.display = 'block';
}

// Hide GDB tutorial modal
function hideGdbTutorial() {
    const gdbTutorialModal = document.getElementById('gdb-tutorial-modal');
    gdbTutorialModal.style.display = 'none';
    overlay.style.display = 'none';
}

// Set up all event listeners
function setupEventListeners() {
    // Main pump controls
    document.getElementById('init-pump').addEventListener('click', resetPump);
    document.getElementById('set-rate').addEventListener('click', setInfusionRate);
    document.getElementById('set-volume').addEventListener('click', setVolumeToInfuse);
    document.getElementById('set-duration').addEventListener('click', setDuration);
    document.getElementById('start-infusion').addEventListener('click', startSimulation);
    document.getElementById('stop-infusion').addEventListener('click', stopSimulation);
    document.getElementById('clear-console').addEventListener('click', clearConsole);
    
    // Admin access
    document.getElementById('admin-access').addEventListener('click', () => showModal(adminModal));
    document.getElementById('login-button').addEventListener('click', validateAdminPassword);
    
    // Admin panel functions
    document.getElementById('set-custom-params').addEventListener('click', setCustomParameters);
    
    // Tutorial
    document.getElementById('close-tutorial').addEventListener('click', hideTutorial);
    document.getElementById('start-challenge').addEventListener('click', hideTutorial);
    
    // Source code
    document.getElementById('view-source').addEventListener('click', showSourceCode);
    document.getElementById('close-source').addEventListener('click', hideSourceCode);
    
    // GDB Tutorial
    document.getElementById('gdb-tutorial').addEventListener('click', showGdbTutorial);
    document.getElementById('close-gdb-tutorial').addEventListener('click', hideGdbTutorial);
    document.getElementById('close-gdb-tutorial-btn').addEventListener('click', hideGdbTutorial);
    
    // Close modals - need to handle multiple close buttons
    document.querySelectorAll('.close').forEach(closeBtn => {
        closeBtn.addEventListener('click', function() {
            // Check which modal this close button belongs to
            const parentModal = this.closest('.modal');
            if (parentModal) {
                hideModal(parentModal);
            }
        });
    });
    
    document.querySelector('.close-admin').addEventListener('click', () => hideModal(adminPanel));
    
    // Close when clicking outside
    overlay.addEventListener('click', () => {
        hideModal(adminModal);
        hideModal(adminPanel);
        hideTutorial();
        hideSourceCode();
        hideGdbTutorial();
    });
}