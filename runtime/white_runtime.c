#include "../include/white/runtime.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

white_signal *white_signal_create(size_t length, double sr) {
    white_signal *s = malloc(sizeof(white_signal));
    s->samples = calloc(length, sizeof(white_iq));
    s->length = length;
    s->sample_rate = sr;
    return s;
}

void white_signal_free(white_signal *s) {
    if (s) {
        free(s->samples);
        free(s);
    }
}

white_signal *white_signal_load_iq(const char *filename, double sample_rate) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    size_t num_samples = file_size / sizeof(white_iq);
    white_signal *s = white_signal_create(num_samples, sample_rate);
    
    fread(s->samples, sizeof(white_iq), num_samples, f);
    fclose(f);
    
    return s;
}

white_spectrum *white_spectrum_create(size_t length, double sr) {
    white_spectrum *sp = malloc(sizeof(white_spectrum));
    sp->magnitude = calloc(length, sizeof(double));
    sp->phase = calloc(length, sizeof(double));
    sp->frequency = calloc(length, sizeof(double));
    sp->length = length;
    sp->sample_rate = sr;
    
    double freq_resolution = sr / length;
    for (size_t i = 0; i < length; i++) {
        sp->frequency[i] = i * freq_resolution;
    }
    
    return sp;
}

void white_spectrum_free(white_spectrum *sp) {
    if (sp) {
        free(sp->magnitude);
        free(sp->phase);
        free(sp->frequency);
        free(sp);
    }
}

static void white_simple_fft(white_iq *in, white_complex *out, size_t size) {
    for (size_t k = 0; k < size; k++) {
        white_complex sum = 0;
        for (size_t n = 0; n < size; n++) {
            double angle = -2.0 * M_PI * k * n / size;
            double re = cos(angle);
            double im = sin(angle);
            white_complex twiddle = re + im * I;
            white_complex sample = in[n].i + in[n].q * I;
            sum += sample * twiddle;
        }
        out[k] = sum;
    }
}

white_spectrum *white_fft(white_signal *s, size_t size) {
    if (!s || size == 0) return NULL;
    
    size_t fft_size = (size < s->length) ? size : s->length;
    white_complex *fft_out = malloc(fft_size * sizeof(white_complex));
    
    white_simple_fft(s->samples, fft_out, fft_size);
    
    white_spectrum *sp = white_spectrum_create(fft_size, s->sample_rate);
    for (size_t i = 0; i < fft_size; i++) {
        sp->magnitude[i] = cabs(fft_out[i]);
        sp->phase[i] = carg(fft_out[i]);
    }
    
    free(fft_out);
    return sp;
}

white_spectrum *white_ifft(white_spectrum *sp) {
    /* Placeholder for IFFT */
    return NULL;
}

double white_spectrum_peak_frequency(white_spectrum *sp) {
    if (!sp || sp->length == 0) return 0;
    
    size_t peak_idx = 0;
    double peak_mag = sp->magnitude[0];
    
    for (size_t i = 1; i < sp->length; i++) {
        if (sp->magnitude[i] > peak_mag) {
            peak_mag = sp->magnitude[i];
            peak_idx = i;
        }
    }
    
    return sp->frequency[peak_idx];
}

double white_spectrum_bandwidth(white_spectrum *sp) {
    if (!sp || sp->length < 2) return 0;
    
    double peak_mag = 0;
    for (size_t i = 0; i < sp->length; i++) {
        if (sp->magnitude[i] > peak_mag) {
            peak_mag = sp->magnitude[i];
        }
    }
    
    double half_power = peak_mag / sqrt(2.0);
    size_t start_idx = 0, end_idx = sp->length - 1;
    
    for (size_t i = 0; i < sp->length; i++) {
        if (sp->magnitude[i] >= half_power) {
            start_idx = i;
            break;
        }
    }
    
    for (size_t i = sp->length - 1; i > 0; i--) {
        if (sp->magnitude[i] >= half_power) {
            end_idx = i;
            break;
        }
    }
    
    return sp->frequency[end_idx] - sp->frequency[start_idx];
}

double white_spectrum_snr(white_spectrum *sp) {
    if (!sp || sp->length == 0) return 0;
    
    double peak_mag = 0, noise_floor = 1e9;
    for (size_t i = 0; i < sp->length; i++) {
        if (sp->magnitude[i] > peak_mag) {
            peak_mag = sp->magnitude[i];
        }
        if (sp->magnitude[i] > 0 && sp->magnitude[i] < noise_floor) {
            noise_floor = sp->magnitude[i];
        }
    }
    
    if (noise_floor == 0) return 100;
    return 20.0 * log10(peak_mag / noise_floor);
}

double white_spectrum_power(white_spectrum *sp) {
    if (!sp || sp->length == 0) return 0;
    
    double power = 0;
    for (size_t i = 0; i < sp->length; i++) {
        power += sp->magnitude[i] * sp->magnitude[i];
    }
    return power / sp->length;
}

white_complex white_complex_add(white_complex a, white_complex b) {
    return a + b;
}

white_complex white_complex_mul(white_complex a, white_complex b) {
    return a * b;
}

white_complex white_complex_conj(white_complex a) {
    return conj(a);
}

double white_complex_magnitude(white_complex a) {
    return cabs(a);
}

double white_complex_phase(white_complex a) {
    return carg(a);
}

white_iq white_iq_from_complex(white_complex c) {
    white_iq iq;
    iq.i = creal(c);
    iq.q = cimag(c);
    return iq;
}

white_complex white_iq_to_complex(white_iq iq) {
    return iq.i + iq.q * I;
}

void white_print_complex(white_complex c) {
    printf("%.6f + %.6fi", creal(c), cimag(c));
}

void white_print_iq(white_iq iq) {
    printf("I=%.6f, Q=%.6f", iq.i, iq.q);
}

void white_print_signal(white_signal *s) {
    if (!s) {
        printf("NULL signal\n");
        return;
    }
    printf("Signal: %zu samples @ %.2f Hz\n", s->length, s->sample_rate);
    for (size_t i = 0; i < (s->length < 10 ? s->length : 10); i++) {
        printf("  [%zu]: ", i);
        white_print_iq(s->samples[i]);
        printf("\n");
    }
    if (s->length > 10) printf("  ... (%zu more samples)\n", s->length - 10);
}

void white_print_spectrum(white_spectrum *sp) {
    if (!sp) {
        printf("NULL spectrum\n");
        return;
    }
    printf("Spectrum: %zu bins @ %.2f Hz SR\n", sp->length, sp->sample_rate);
    for (size_t i = 0; i < (sp->length < 10 ? sp->length : 10); i++) {
        printf("  [%zu] %.2f Hz: mag=%.6f, phase=%.6f\n", i, sp->frequency[i], sp->magnitude[i], sp->phase[i]);
    }
    if (sp->length > 10) printf("  ... (%zu more bins)\n", sp->length - 10);
}
void white_signal_save_iq(white_signal *s, const char *filename) {
    if (!s || !filename) return;

    FILE *f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Error: Failed to open file for writing\n");
        return;
    }
    // Implement IQ signal saving logic here
    fclose(f);
}