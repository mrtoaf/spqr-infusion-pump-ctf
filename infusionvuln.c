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
    uint16_t rate_ml_per_hour;  // Current flow rate
    uint16_t volume_to_infuse_ml;  // Total volume to deliver
    uint16_t volume_infused_ml;    // Current delivered volume
    uint8_t is_running;
    uint8_t is_admin_mode;      // Flag to indicate admin-set parameters
    uint32_t elapsed_time_ms;    // Time since start of infusion
} InfusionPumpState;

InfusionPumpState pump_state;

// Initialize the pump with default values
void init_pump() {
    strcpy(pump_state.patient_id, "UNKNOWN");
    strcpy(pump_state.medication, "SALINE");
    pump_state.rate_ml_per_hour = DEFAULT_RATE_ML_PER_HOUR;
    pump_state.volume_to_infuse_ml = DEFAULT_VOLUME_ML;
    pump_state.volume_infused_ml = 0;
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
    
    pump_state.volume_to_infuse_ml = new_volume;
    printf("Volume to infuse set to %d ml\n", new_volume);
    return 0;
}

// Calculate how much fluid should be delivered based on elapsed time
uint16_t calculate_volume_to_deliver(uint32_t time_increment_ms) {

    uint32_t volume_microliters = pump_state.rate_ml_per_hour * time_increment_ms * 1000 / 3600000;

    // Convert back to milliliters (truncating fractions)
    uint16_t volume_ml = (uint16_t)(volume_microliters / 1000);
    
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
    uint16_t volume_to_deliver = calculate_volume_to_deliver(time_increment_ms);
    
    // Update the infused volume
    pump_state.volume_infused_ml += volume_to_deliver;
    
    // Check if infusion is complete
    if (pump_state.volume_infused_ml >= pump_state.volume_to_infuse_ml) {
        pump_state.is_running = 0;
        printf("Infusion complete! Delivered %d ml\n", pump_state.volume_infused_ml);
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
    printf("Volume to infuse: %d ml\n", pump_state.volume_to_infuse_ml);
    
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
void admin_set_custom_parameters(uint16_t rate, uint32_t duration_ms) {
    printf("ADMIN: Setting custom parameters\n");
    printf("ADMIN: Rate: %d ml/hr, Duration: %u ms\n", rate, duration_ms);
    
    // For "admin" function, bypass normal checks
    pump_state.rate_ml_per_hour = rate;
    
    // Calculate how much will be delivered with these parameters
    uint16_t calculated_volume = calculate_volume_to_deliver(duration_ms);
    printf("ADMIN: Calculated delivery volume: %d ml\n", calculated_volume);
    
    // Set the volume to infuse based on the calculated value
    pump_state.volume_to_infuse_ml = calculated_volume;
}

// Simple menu-driven interface for the infusion pump
int main() {
    char buffer[100];
    int choice;
    int authenticated = 0;
    uint16_t rate;
    uint32_t duration;
    
    init_pump();
    
    while (1) {
        printf("\n=== Infusion Pump Control System ===\n");
        printf("1. Set Infusion Rate\n");
        printf("2. Set Volume to Infuse\n");
        printf("3. Start Infusion\n");
        printf("4. Run Infusion Cycle (5 minutes)\n");
        printf("5. Initialize Pump\n");
        printf("6. [Reserved]\n");
        printf("7. Admin Access\n");
        printf("8. Exit\n");
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
                
            case 3: // Start Infusion
                start_infusion();
                break;
                
            case 4: // Run Infusion Cycle
                run_infusion_cycle(300000); // 5 minutes
                printf("Time elapsed: %d ms, Volume infused: %d ml\n", 
                       pump_state.elapsed_time_ms, pump_state.volume_infused_ml);
                break;
                
            case 5: // Initialize Pump
                init_pump();
                break;
                
            case 6: // Reserved for future use
                printf("This feature is not implemented yet.\n");
                break;
                
            case 7: // Admin Access
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
                        
                        printf("Enter duration (ms): ");
                        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
                        duration = (uint32_t)atol(buffer);
                        
                        admin_set_custom_parameters(rate, duration);
                        break;
                        
                    case 2: // Return to Main Menu
                        break;
                        
                    default:
                        printf("Invalid admin choice\n");
                        break;
                }
                break;
                
            case 8: // Exit
                printf("Exiting program. Goodbye!\n");
                return 0;
                
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
    
    return 0;
}
