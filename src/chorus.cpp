#include <random>
#include <vector>
#include <cmath>
#include <tuple>

#include "effects.hpp"

const double PI  = 3.141592653589793238463;
const size_t DEFAULT_SAMPLE_RATE = 44100;

Chorus Chorus::create_random(
    size_t voice_count,             //Number of voices to add
    size_t max_delay,               //The maximum delay of the voices
    double LFO_freq_base,           //Base frequency of the pitch LFO
    double LFO_freq_variance,       //The range of the pitch LFO frequencies [base-variance, base+variance]
    double LFO_amp_base,            //Base amplitude for pitch LFO
    double LFO_amp_variance,        //amplitude variance for pitch LFO
    double pitch_variance,          //the variance of the midpoint of the pitch LFO
    double gain                     //The gain of the added voices 1...no volume difference
){
    //Random number distributions
    std::uniform_real_distribution<double> freq_unif(LFO_freq_base-LFO_freq_variance, LFO_freq_base+LFO_freq_variance);
    std::uniform_real_distribution<double> amp_unif(LFO_amp_base-LFO_amp_variance, LFO_amp_base+LFO_amp_variance);
    std::uniform_int_distribution<size_t> delay_unif(0, max_delay);
    std::uniform_real_distribution<double> pos_unif(0, 2 * PI);
    std::uniform_real_distribution<double> ps_unif(1-pitch_variance, 1+pitch_variance);
    std::random_device rd;
    std::default_random_engine re(rd());

    std::vector<double> voice_LFO_pos;
    std::vector<double> voice_LFO_delta;
    std::vector<double> voice_LFO_amp;
    std::vector<double> voice_pitch;
    std::vector<Delay> delays;

    for(size_t i=0; i<voice_count; ++i){
        voice_LFO_delta.push_back((2*PI*freq_unif(re))/DEFAULT_SAMPLE_RATE);
        voice_LFO_amp.push_back(amp_unif(re));
        std::vector<size_t> d={delay_unif(re)};
        std::vector<double> g={gain};
        delays.emplace_back(d,g);
        voice_pitch.push_back(ps_unif(re));
        voice_LFO_pos.push_back(pos_unif(re));
    }

    Chorus ret(voice_LFO_pos,voice_LFO_delta,voice_LFO_amp,voice_pitch,delays);
    return ret;
}
//Uniform = liniar, because its the easiest to implement
Chorus Chorus::create_uniform(
    size_t voice_count,             //Number of voices to add
    size_t max_delay,               //The maximum delay of the voices
    double LFO_freq_base,           //Base frequency of the pitch LFO
    double LFO_freq_variance,       //The range of the pitch LFO frequencies [base-variance, base+variance]
    double LFO_amp_base,            //Base amplitude for pitch LFO
    double LFO_amp_variance,        //amplitude variance for pitch LFO
    double pitch_variance,          //the variance of the midpoint of the pitch LFO
    double gain                     //The gain of the added voices 1...no volume difference
){
    std::vector<double> voice_LFO_pos;
    std::vector<double> voice_LFO_delta;
    std::vector<double> voice_LFO_amp;
    std::vector<double> voice_pitch;
    std::vector<Delay> delays;

    for(size_t i=0; i<voice_count; ++i){
        //Lower frequency ~ higher amp
        double freq = (LFO_freq_base - LFO_freq_variance) + ((2 * LFO_freq_variance)/(voice_count-1))*i;
        voice_LFO_delta.push_back((2*PI*freq)/DEFAULT_SAMPLE_RATE);
        voice_LFO_amp.push_back((LFO_amp_base + LFO_amp_variance) - ((2 * LFO_amp_variance)/(voice_count-1))*i);
        voice_pitch.push_back((1-pitch_variance)+i*(2*pitch_variance)/(voice_count-1));
        std::vector<size_t> d={(max_delay/(voice_count-1))*i};
        std::vector<double> g={gain};
        delays.emplace_back(d,g);
    }
    voice_LFO_pos.resize(voice_count,0);
    Chorus ret(voice_LFO_pos,voice_LFO_delta,voice_LFO_amp,voice_pitch,delays);
    return ret;
}
void Chorus::operator()(FFTwrapper &fft){
    fft.c2r_transform();

    //Copy samples to save them, so we dont overwrite them
    std::vector<double> res(fft.get_real_ptr(),fft.get_real_ptr()+fft.get_real_c());

    fft.r2c_transform();
    
    //Copy spectrum to change it multiple times
    std::vector<std::tuple<double,double>> spectrum;
    for(size_t j=0; j<fft.get_complex_c(); ++j){
        spectrum.push_back(std::make_tuple(fft.get_complex_ptr()[j][0],fft.get_complex_ptr()[j][1]));
    }

    for(size_t i=0; i<voice_LFO_pos_.size();++i){
        double pitch = voice_pitch_[i] * std::exp2(voice_LFO_amp_[i]*std::sin(voice_LFO_pos_[i]));
        PitchShift::shift_complex(spectrum, fft, pitch);
        voice_LFO_pos_[i] += voice_LFO_delta_[i] * fft.get_real_c();
        //Reseting the pos to stay in range [0,2 pi], so we dont lose precision
        if(voice_LFO_pos_[i] > 2*PI)
            voice_LFO_pos_[i] -= 2*PI;
        
        fft.c2r_transform();        

        delays_[i](fft.get_real_ptr(), res.data(), fft.get_real_c());    

    }
    fft.is_complex_state_ = false;
    //overwrite with created data
    for(size_t i=0; i<res.size(); ++i){
        fft.get_real_ptr()[i] = res[i];
    }
}