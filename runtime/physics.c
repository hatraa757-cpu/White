#include "../include/white/runtime.h"
#include <math.h>
#include <stdio.h>

/* Physics constants */
const double SPEED_OF_LIGHT = 299792458.0;        /* m/s */
const double PLANCK_CONSTANT = 6.62607015e-34;    /* J·s */
const double BOLTZMANN_CONSTANT = 1.380649e-23;   /* J/K */
const double EPSILON_0 = 8.854187817e-12;         /* F/m */
const double MU_0 = 1.256637061e-6;               /* H/m */

/* RF/DSP physics calculations */
double white_wavelength_from_frequency(double frequency) {
    if (frequency <= 0.0) return 0.0;
    return SPEED_OF_LIGHT / frequency;
}

double white_frequency_from_wavelength(double wavelength) {
    if (wavelength <= 0.0) return 0.0;
    return SPEED_OF_LIGHT / wavelength;
}

/* Free space path loss (Friis equation) */
double white_friis_path_loss(double frequency, double distance) {
    if (frequency <= 0.0 || distance <= 0.0) return 0.0;
    
    double wavelength = SPEED_OF_LIGHT / frequency;
    double path_loss_linear = pow(wavelength / (4.0 * M_PI * distance), 2);
    return 10.0 * log10(path_loss_linear);
}

/* VSWR from reflection coefficient */
double white_vswr_from_reflection(double reflection_coeff) {
    if (reflection_coeff >= 1.0) return 999.0;
    if (reflection_coeff < 0.0) reflection_coeff = -reflection_coeff;
    return (1.0 + reflection_coeff) / (1.0 - reflection_coeff);
}

/* Return loss from reflection coefficient */
double white_return_loss(double reflection_coeff) {
    if (reflection_coeff <= 0.0) return 999.0;
    if (reflection_coeff > 1.0) reflection_coeff = 1.0;
    return -20.0 * log10(reflection_coeff);
}

/* Impedance matching */
double white_impedance_free_space(void) {
    return sqrt(MU_0 / EPSILON_0);
}

/* Antenna gain to EIRP */
double white_eirp_from_power_gain(double power_dbm, double gain_db) {
    return power_dbm + gain_db;
}

/* Received power calculation */
double white_received_power(double transmit_power_dbm, double tx_gain_db, 
                           double rx_gain_db, double frequency, double distance) {
    double path_loss = white_friis_path_loss(frequency, distance);
    return transmit_power_dbm + tx_gain_db + rx_gain_db - fabs(path_loss);
}

/* SNR to BER estimation (simplified) */
double white_ber_from_snr(double snr_db) {
    double snr_linear = pow(10.0, snr_db / 10.0);
    if (snr_linear > 20.0) {
        return 0.5 * erfc(sqrt(snr_linear));
    }
    return 1.0 - erf(sqrt(snr_linear) / sqrt(2.0));
}

/* Noise power calculation */
double white_noise_power(double temp_k, double bandwidth_hz) {
    if (bandwidth_hz <= 0.0) return 0.0;
    double power_watts = BOLTZMANN_CONSTANT * temp_k * bandwidth_hz;
    return 10.0 * log10(power_watts * 1000.0);  /* Convert to dBm */
}

/* Q factor calculation */
double white_q_factor(double frequency, double bandwidth) {
    if (bandwidth <= 0.0) return 0.0;
    return frequency / bandwidth;
}
double white_bandwidth_from_q(double frequency, double q_factor) {
    if (q_factor <= 0.0) return 0.0;
    return frequency / q_factor;
}
