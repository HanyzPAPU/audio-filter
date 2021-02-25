#ifndef EFFECTS_HPP_
#define EFFECTS_HPP_

#include <istream>
#include <vector>
#include <tuple>

#include "io.hpp"
#include "fft_wrapper.hpp"

/*
 *  Frequency is normalized to be independent of sample rate, calculating with sample rate of 44 100 Hz
 *      Frequency of 1 means that there is one period in 44 100 samples
 *      Frequency n means there are n periods in 44 100 samples
 */

class PitchShift{
public:
    ///Ammount must be >1;
    PitchShift(double ammount):ammount_(ammount){};
    void operator()(FFTwrapper &fft);
    ///Pitch shifts the data into the output FFTwrapper complex buffer, overwriting any data stored
    static void shift_complex(const std::vector<std::tuple<double,double>> &in, FFTwrapper &out_fft, double ammount);
private:
    double ammount_;
};

PitchShift make_pitchshift(std::istream &is, const IO &io);


class Lowpass{
public:
    ///Cutoff = what percenatge is cut off
    //Cutoff must be between 0 and 1;
    Lowpass(double cutoff): cutoff_(cutoff){};
    void operator()(FFTwrapper &fft);
private:
    double cutoff_;
};

Lowpass make_lowpass(std::istream &is, const IO &io);

class Highpass{
public:
    ///Cutoff = what percenatge is cut off
    //Cutoff must be between 0 and 1;
    Highpass(double cutoff): cutoff_(cutoff){};
    void operator()(FFTwrapper &fft);
private:
    double cutoff_;
};

Highpass make_highpass(std::istream &is, const IO &io);

///Simple delay of samples, accounting for 50 % overlap
class Delay{
public:
    Delay(
        const std::vector<size_t> &delays,   //delay in samples per one delay "instance"
        const std::vector<double> &gains     //gains of the individual delay "instances"
    );
    void operator()(FFTwrapper &fft);
    //Input and output can be the same pointer
    void operator()(double *input, double* output, size_t size);
    static Delay create_linear(size_t delay, size_t count);
    static Delay create_exp(size_t delay, size_t count, double base);
private:
    std::vector<double> older_data_, newer_data_;
    std::vector<size_t> delays_;
    std::vector<double> gains_;
};

Delay make_delay(std::istream &is, const IO &io);

class Chorus{
public:
    Chorus(
        const std::vector<double> &voice_LFO_pos,
        const std::vector<double> &voice_LFO_delta,
        const std::vector<double> &voice_LFO_amp,
        const std::vector<double> &voice_pitch,
        const std::vector<Delay> &delays
    ):voice_LFO_pos_(voice_LFO_pos),
      voice_LFO_delta_(voice_LFO_delta),
      voice_LFO_amp_(voice_LFO_amp),
      voice_pitch_(voice_pitch),
      delays_(delays)
    {}
    static Chorus create_random(
        size_t voice_count,             //Number of voices to add
        size_t max_delay,               //The maximum delay of the voices
        double LFO_freq_base,           //Base frequency of the pitch LFO
        double LFO_freq_variance,       //The range of the pitch LFO frequencies [base-variance, base+variance]
        double LFO_amplitude_base,      //Base amp for pitch LFO
        double LFO_amplitude_variance,  //amp variance for pitch LFO
        double pitch_variance,          //the variance of the midpoint of the pitch LFO
        double gain                     //The gain of the added voices 1...no volume difference
    );
    static Chorus create_uniform(
        size_t voice_count,             //Number of voices to add
        size_t max_delay,               //The maximum delay of the voices
        double LFO_freq_base,           //Base frequency of the pitch LFO
        double LFO_freq_variance,       //The range of the pitch LFO frequencies [base-variance, base+variance]
        double LFO_amplitude_base,      //Base amp for pitch LFO
        double LFO_amplitude_variance,  //amp variance for pitch LFO
        double pitch_variance,          //the variance of the midpoint of the pitch LFO
        double gain                     //The gain of the added voices 1...no volume difference
    );
    void operator()(FFTwrapper &fft);
private:
    std::vector<double> voice_LFO_pos_;
    std::vector<double> voice_LFO_delta_;
    std::vector<double> voice_LFO_amp_;
    std::vector<double> voice_pitch_;
    std::vector<Delay> delays_;
};

Chorus make_chorus(std::istream &is, const IO &io);

#endif