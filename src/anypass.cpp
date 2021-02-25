#include "effects.hpp"
#include "fft_wrapper.hpp"

void Lowpass::operator()(FFTwrapper &fft){
    fft.r2c_transform();
    for(size_t i = (size_t)(fft.get_complex_c()*(1-cutoff_)); i<fft.get_complex_c(); ++i){
        fft.get_complex_ptr()[i][0]=0;
        fft.get_complex_ptr()[i][1]=0;
    }
}

void Highpass::operator()(FFTwrapper &fft){
    fft.r2c_transform();
    for(size_t i = 0; i<((size_t)fft.get_complex_c()*cutoff_); ++i){
        fft.get_complex_ptr()[i][0]=0;
        fft.get_complex_ptr()[i][1]=0;
    }
}