#include <fftw3.h>
#include <vector>
#include <string>
#include <fstream>
#include <functional>
#include <sstream>
#include <iostream>
#include <map>
#include <experimental/filesystem>

#include "fft_wrapper.hpp"
#include "io.hpp"
#include "effects.hpp"
#include "window.hpp"

void parse_args(
    const std::vector<std::string> &args,   // Vector holding the arguments
    std::string &i_path,                    // Path of the input file will be loaded here
    std::string &o_path,                    // Path of the output file will be loaded here
    std::string &c_path,                    // Path of the config file will be loaded here
    size_t &window_size                     // The size of the window
    ){                   

    for(size_t i=0; i<args.size(); ++i){
        if(i==args.size()-1){
            throw std::out_of_range("Wrong argument format!");
        }
        if(args[i]=="-i"){
            if(!std::experimental::filesystem::exists(args[++i])){
                throw std::invalid_argument("Invalid input file path: "+args[i]);
            }
            i_path = args[i];
        }
        else if(args[i]=="-o"){
            o_path = args[++i];
        }
        else if(args[i]=="-c"){
            if(!std::experimental::filesystem::exists(args[++i])){
                throw std::invalid_argument("Invalid config file path: "+args[i]);
            }
            c_path = args[i];
        }
        else if(args[i]=="-w"){
            window_size = std::stoull(args[++i]);
        }
        else{
            throw std::invalid_argument("Invalid argument!");
        }
    }
}
///Creates a vector of filters specified in config file
std::vector<std::function<void(FFTwrapper &)>> make_filters(const std::string &c_path, const IO &io){
    std::ifstream ifs(c_path);
    if(!ifs.is_open())
        throw std::invalid_argument("Config file could not be opened!");
    std::string line;
    std::vector<std::function<void(FFTwrapper &)>> ret;

    std::map<
        std::string,
        std::function<
            std::function<void(FFTwrapper &)>(std::istream&, const IO &)
        >
    > makers = {
        {"lowpass",make_lowpass},
        {"highpass",make_highpass},
        {"delay", make_delay},
        {"pitchshift",make_pitchshift},
        {"chorus",make_chorus}
    };
    
    while (std::getline(ifs,line)){
        if(line.empty() || line[0]=='#')
            continue;
        std::istringstream iss(line);
        std::string name;
        iss>>name;
        ret.push_back(makers[name](iss, io));
    }
    return ret;
}

int main(int argc, char **argv){
    try{
        std::vector<std::string> args(argv+1, argv+argc);
        std::string i_path, o_path, c_path;
        size_t size = 8192;
        parse_args(args, i_path, o_path, c_path, size);
        IO io(i_path, o_path);

        FFTwrapper fft(size);
        std::vector<std::function<void(FFTwrapper &)>> filters;
        try{
           filters  = make_filters(c_path, io);
        }
        catch(...){
            throw std::invalid_argument("Bad config file format!");
        }
        sf_count_t c;
        Window window(size);
        WriteOverlap write(size);

        while((c = window(fft.get_real_ptr(),io))){
                for(auto &&f: filters)
                    f(fft);
                fft.c2r_transform();
                write(c,fft.get_real_ptr(),io);
        }  
    }
    catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
        fftw_cleanup();
        return 1;
    }
    catch(...){
        std::cerr<<"Unknown error"<<std::endl;
        fftw_cleanup();
        return 2;
    }
    fftw_cleanup();
    return 0;
}