#include <istream>
#include <string>
#include <sndfile.hh>

#include "io.hpp"
#include "effects.hpp"
#include "fft_wrapper.hpp"


PitchShift make_pitchshift(std::istream &is, const IO &io){
    double ammount;
    is>>ammount;
    PitchShift ps(ammount);
    return ps;
}
Lowpass make_lowpass(std::istream &is, const IO &io){
    double cutoff;
    is>>cutoff;
    Lowpass lp(cutoff);
    return lp;
}
Highpass make_highpass(std::istream &is, const IO &io){
    double cutoff;
    is>>cutoff;
    Highpass hp(cutoff);
    return hp;
}

Delay make_delay(std::istream &is, const IO &io){
    std::string mode;
    is>>mode;
    std::vector<size_t> delays;
    std::vector<double> gains;
    if(mode=="auto"){
        std::string type;
        size_t delay, count;
        is>>type>>delay>>count;
        //ms -> samples
        delay *= io.get_info().samplerate/1000;
        if(type=="linear"){
            return Delay::create_linear(delay, count);
        }
        else if(type=="exp"){
            double base;
            is>>base;
            return Delay::create_exp(delay, count, base);
        }
        else{
            throw std::invalid_argument("Bad delay type: "+type);
        }
    }
    else if(mode=="manual"){
        size_t delay;
        double gain;
        while(is){
            is>>delay>>gain;
            //ms -> samples
            delays.push_back(delay*io.get_info().samplerate/1000);
            gains.push_back(gain);
        }
        return Delay(delays, gains);
    }
    throw std::invalid_argument("Bad delay mode: "+mode);
}

Chorus make_chorus(std::istream &is, const IO &io){
    size_t voice_count, max_delay;
    double LFO_freq_base, LFO_freq_variance, LFO_amplitude_base,
           LFO_amplitude_variance, pitch_variance, gain;
    std::string mode;
    is>>mode>>voice_count>>max_delay
      >>LFO_freq_base>>LFO_freq_variance
      >>LFO_amplitude_base>>LFO_amplitude_variance
      >>pitch_variance>>gain;
      
    //ms -> samples
    max_delay *= io.get_info().samplerate/1000;

    //from Hz to normalized freq
    LFO_freq_base *= io.get_info().samplerate/44100;

    //from Hz to normalized freq
    LFO_freq_variance *= io.get_info().samplerate/44100;

    if(mode=="random"){
        return Chorus::create_random(
            voice_count,
            max_delay,
            LFO_freq_base,
            LFO_freq_variance,
            LFO_amplitude_base,
            LFO_amplitude_variance,
            pitch_variance,
            gain
        );
    }
    else if(mode=="uniform"){
        return Chorus::create_uniform(
            voice_count,
            max_delay,
            LFO_freq_base,
            LFO_freq_variance,
            LFO_amplitude_base,
            LFO_amplitude_variance,
            pitch_variance,
            gain
        );
    }
    else{
        throw std::invalid_argument("Bad Chorus mode: "+mode);
    }
}