#ifndef WHITE_RUNTIME_H
#define WHITE_RUNTIME_H

#include <complex.h>
#include <stddef.h>
#include <stdint.h>

/* Complex number type */
typedef double complex white_complex;

/* IQ sample */
typedef struct {
    double i;
    double q;
} white_iq;

/* Signal */
typedef struct {
    white_iq *samples;
    size_t length;
    double sample_rate;
} white_signal;

/* Spectrum */
typedef struct {
    double *magnitude;
    double *phase;
    double *frequency;
    size_t length;
    double sample_rate;
} white_spectrum;

/* Signal operations */
white_signal *white_signal_create(size_t length, double sr);
void white_signal_free(white_signal *s);
white_signal *white_signal_load_iq(const char *filename, double sample_rate);

/* Spectrum operations */
white_spectrum *white_spectrum_create(size_t length, double sr);
void white_spectrum_free(white_spectrum *sp);
white_spectrum *white_fft(white_signal *s, size_t size);
white_spectrum *white_ifft(white_spectrum *sp);

/* Analysis */
double white_spectrum_peak_frequency(white_spectrum *sp);
double white_spectrum_bandwidth(white_spectrum *sp);
double white_spectrum_snr(white_spectrum *sp);
double white_spectrum_power(white_spectrum *sp);

/* Complex arithmetic */
white_complex white_complex_add(white_complex a, white_complex b);
white_complex white_complex_mul(white_complex a, white_complex b);
white_complex white_complex_conj(white_complex a);
double white_complex_magnitude(white_complex a);
double white_complex_phase(white_complex a);

/* Math utilities */
white_iq white_iq_from_complex(white_complex c);
white_complex white_iq_to_complex(white_iq iq);

/* Print utilities */
void white_print_complex(white_complex c);
void white_print_iq(white_iq iq);
void white_print_signal(white_signal *s);
void white_print_spectrum(white_spectrum *sp);
void white_print_signal_to_file(white_signal *s, const char *filename);
void

#endif
