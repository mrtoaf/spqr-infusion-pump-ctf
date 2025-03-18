/*
 * infusionvuln.c - SPQR Infusion Pump Controller
 * 
 * WARNING: This firmware contains intentional vulnerabilities for educational purposes.
 * DO NOT use for actual medical devices\!
 */

// SPQR Infusion Pump Firmware (Decompiled)
// Model: SPQR-IPF-2025
// Version: 1.0.3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

// Device configuration
#define MAX_RATE_ML_PER_HOUR 1000
#define DEFAULT_RATE_ML_PER_HOUR 100
#define MAX_VOLUME_ML 1000
#define DEFAULT_VOLUME_ML 500
#define SAFETY_CHECK_INTERVAL_MS 5000
#define ADMIN_PASSWORD "SPQR2025"

// Device state
typedef struct {
    char patient_id[16];
    char medication[32];
    uint16_t rate_ml_per_hour;     // Current flow rate
    double volume_to_infuse_ml;    // Total volume to deliver
    double volume_infused_ml;      // Current delivered volume
    uint32_t duration_seconds;     // Duration of infusion in seconds
    uint8_t is_running;
    uint8_t is_admin_mode;         // Flag to indicate admin-set parameters
    uint32_t elapsed_time_ms;      // Time since start of infusion
} InfusionPumpState;

InfusionPumpState pump_state;

// Initialize the pump with default values
void init_pump() {
    strcpy(pump_state.patient_id, "UNKNOWN");
    strcpy(pump_state.medication, "SALINE");
    pump_state.rate_ml_per_hour = DEFAULT_RATE_ML_PER_HOUR;
    pump_state.volume_to_infuse_ml = (double)DEFAULT_VOLUME_ML;
    pump_state.volume_infused_ml = 0.0;
    pump_state.duration_seconds = 300; // Default 5 minutes
    pump_state.is_running = 0;
    pump_state.is_admin_mode = 0;
    pump_state.elapsed_time_ms = 0;
    
    printf("Pump initialized with default settings\n");
}

// Validate and set the infusion rate
int set_infusion_rate(uint16_t new_rate) {
    if (new_rate > MAX_RATE_ML_PER_HOUR) {
        printf("ERROR: Rate exceeds maximum allowed (%d ml/hr)\n", MAX_RATE_ML_PER_HOUR);
        return -1;
    }
    
    pump_state.rate_ml_per_hour = new_rate;
    printf("Infusion rate set to %d ml/hr\n", new_rate);
    return 0;
}

// Validate and set the volume to infuse
int set_volume_to_infuse(uint16_t new_volume) {
    if (new_volume > MAX_VOLUME_ML) {
        printf("ERROR: Volume exceeds maximum allowed (%d ml)\n", MAX_VOLUME_ML);
        return -1;
    }
    
    pump_state.volume_to_infuse_ml = (double)new_volume;
    printf("Volume to infuse set to %.2f ml\n", pump_state.volume_to_infuse_ml);
    return 0;
}

// Set duration in seconds
int set_duration(uint32_t seconds) {
    pump_state.duration_seconds = seconds;
    printf("Duration set to %u seconds\n", seconds);
    return 0;
}

// Calculate how much fluid should be delivered based on elapsed time
double calculate_volume_to_deliver(uint32_t time_increment_ms) {

    // Vulnerable calculation - integer overflow with large values
    uint32_t volume_microliters = pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;

    // Convert back to milliliters with decimal precision
    double volume_ml = (double)volume_microliters / 1000.0;
    
    return volume_ml;
}

// Infusion process for a given time increment
void run_infusion_cycle(uint32_t time_increment_ms) {
    if (!pump_state.is_running) {
        return;
    }
    
    // Update elapsed time
    pump_state.elapsed_time_ms += time_increment_ms;
    
    // Calculate volume to deliver during this cycle
    double volume_to_deliver = calculate_volume_to_deliver(time_increment_ms);
    
    // Update the infused volume
    pump_state.volume_infused_ml += volume_to_deliver;
    
    // Check if infusion is complete
    if (pump_state.volume_infused_ml >= pump_state.volume_to_infuse_ml) {
        pump_state.is_running = 0;
        printf("Infusion complete! Delivered %.2f ml\n", pump_state.volume_infused_ml);
    }
}


// Start the infusion
void start_infusion() {
    if (pump_state.is_running) {
        printf("Infusion already in progress\n");
        return;
    }
    
    printf("Starting infusion...\n");
    printf("Patient ID: %s\n", pump_state.patient_id);
    printf("Medication: %s\n", pump_state.medication);
    printf("Rate: %d ml/hr\n", pump_state.rate_ml_per_hour);
    printf("Volume to infuse: %.2f ml\n", pump_state.volume_to_infuse_ml);
    
    // Set admin mode flag if we're using admin-set parameters
    if (pump_state.volume_to_infuse_ml > MAX_VOLUME_ML || 
        pump_state.rate_ml_per_hour > MAX_RATE_ML_PER_HOUR) {
        printf("NOTICE: Using admin parameters that exceed normal limits\n");
        pump_state.is_admin_mode = 1;
    } else {
        pump_state.is_admin_mode = 0;
    }
    
    pump_state.is_running = 1;
}

// Validate admin password
int validate_admin_password(const char* password) {
    if (password == NULL) return 0;
    
    if (strcmp(password, ADMIN_PASSWORD) == 0) {
        printf("Admin authentication successful\n");
        return 1;
    } else {
        printf("Admin authentication failed\n");
        return 0;
    }
}

// Special admin function to set custom parameters
void admin_set_custom_parameters(uint16_t rate, uint32_t duration_seconds) {
    printf("ADMIN: Setting custom parameters\n");
    printf("ADMIN: Rate: %d ml/hr, Duration: %u seconds\n", rate, duration_seconds);
    
    // For "admin" function, bypass normal checks
    pump_state.rate_ml_per_hour = rate;
    pump_state.duration_seconds = duration_seconds;
    
    // Convert duration to milliseconds
    uint32_t duration_ms = duration_seconds * 1000;
    
    // Calculate how much should be delivered with these parameters
    double calculated_volume = calculate_volume_to_deliver(duration_ms);
    
    // Calculate expected volume using mathematical formula for comparison
    // This uses double to avoid overflow issues
    double expected_volume = (double)rate * (double)duration_ms / 3600000.0;
    
    printf("ADMIN: Firmware calculated volume: %.2f ml\n", calculated_volume);
    printf("ADMIN: Mathematically expected volume: %.2f ml\n", expected_volume);
    
    if (fabs(calculated_volume - expected_volume) > 1.0) {
        printf("ADMIN WARNING: There is a discrepancy between calculated and expected volumes!\n");
        printf("This may indicate a numeric calculation issue in the firmware.\n");
    }
    
    // Set the volume to infuse based on the calculated value
    pump_state.volume_to_infuse_ml = calculated_volume;
    
    // Reset the infused volume and elapsed time
    pump_state.volume_infused_ml = 0.0;
    pump_state.elapsed_time_ms = 0;
    
    // Don't automatically start infusion - require user to press Start Infusion
    printf("Parameters set. Press Start Infusion to begin.\n");
}

// Display current pump status
void display_status() {
    printf("\n--- Current Pump Status ---\n");
    printf("Patient ID: %s\n", pump_state.patient_id);
    printf("Medication: %s\n", pump_state.medication);
    printf("Rate: %d ml/hr\n", pump_state.rate_ml_per_hour);
    printf("Volume to infuse: %.2f ml\n", pump_state.volume_to_infuse_ml);
    printf("Volume infused: %.2f ml\n", pump_state.volume_infused_ml);
    printf("Duration: %u seconds\n", pump_state.duration_seconds);
    printf("Status: %s\n", pump_state.is_running ? "Running" : "Stopped");
    printf("Mode: %s\n", pump_state.is_admin_mode ? "Admin" : "Normal");
    printf("Elapsed time: %u seconds\n", pump_state.elapsed_time_ms / 1000);
    printf("--------------------------\n");
}

// Simple menu-driven interface for the infusion pump
int main() {
    char buffer[100];
    int choice;
    int authenticated = 0;
    uint16_t rate;
    uint32_t duration_seconds;
    
    init_pump();
    
    while (1) {
        // Display current settings
        display_status();
        
        printf("\n=== SPQR Infusion Pump Control System ===\n");
        printf("1. Set Infusion Rate\n");
        printf("2. Set Volume to Infuse\n");
        printf("3. Set Duration\n");
        printf("4. Start Infusion\n");
        printf("5. Reset Pump\n");
        printf("6. Admin Access\n");
        printf("7. Exit\n");
        printf("Enter choice: ");
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        choice = atoi(buffer);
        
        switch (choice) {
            case 1: // Set Infusion Rate
                printf("Enter rate (ml/hr): ");
                if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
                rate = (uint16_t)atoi(buffer);
                set_infusion_rate(rate);
                break;
                
            case 2: // Set Volume to Infuse
                printf("Enter volume (ml): ");
                if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
                uint16_t volume = (uint16_t)atoi(buffer);
                set_volume_to_infuse(volume);
                break;
                
            case 3: // Set Duration
                printf("Enter duration (seconds): ");
                if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
                duration_seconds = (uint32_t)atol(buffer);
                set_duration(duration_seconds);
                break;
                
            case 4: // Start Infusion
                start_infusion();
                // Only run if the pump is actually started
                if (pump_state.is_running) {
                    // Use the stored duration in the pump state
                    uint32_t infusion_duration_ms = pump_state.duration_seconds * 1000;
                    
                    run_infusion_cycle(infusion_duration_ms);
                    printf("Time elapsed: %u seconds, Volume infused: %.2f ml\n", 
                           pump_state.elapsed_time_ms / 1000, pump_state.volume_infused_ml);
                    
                    // Show expected mathematical result as well for comparison
                    double expected_volume = (double)pump_state.rate_ml_per_hour * 
                                            (double)infusion_duration_ms / 3600000.0;
                    printf("Mathematically expected volume: %.2f ml\n", expected_volume);
                    
                    // Show warning if there's a significant discrepancy
                    if (fabs(pump_state.volume_infused_ml - expected_volume) > 1.0) {
                        printf("WARNING: Discrepancy detected between actual and expected volumes!\n");
                    }
                }
                break;
                
            case 5: // Reset Pump
                init_pump();
                break;
                
            case 6: // Admin Access
                if (!authenticated) {
                    printf("Enter admin password: ");
                    if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
                    // Remove newline character
                    buffer[strcspn(buffer, "\n")] = 0;
                    authenticated = validate_admin_password(buffer);
                    
                    if (!authenticated) {
                        printf("Invalid password. Access denied.\n");
                        break;
                    }
                }
                
                // Admin menu (only accessible with valid password)
                printf("\n=== Admin Menu ===\n");
                printf("1. Set Custom Parameters\n");
                printf("2. Return to Main Menu\n");
                printf("Enter admin choice: ");
                
                if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
                int admin_choice = atoi(buffer);
                
                switch (admin_choice) {
                    case 1: // Set Custom Parameters
                        printf("Enter rate (ml/hr): ");
                        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
                        rate = (uint16_t)atoi(buffer);
                        
                        printf("Enter duration (seconds): ");
                        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
                        duration_seconds = (uint32_t)atol(buffer);
                        
                        // Set custom parameters but don't start infusion
                        admin_set_custom_parameters(rate, duration_seconds);
                        break;
                        
                    case 2: // Return to Main Menu
                        break;
                        
                    default:
                        printf("Invalid admin choice\n");
                        break;
                }
                break;
                
            case 7: // Exit
                printf("Exiting program. Goodbye!\n");
                return 0;
                
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
    
    return 0;
}
