bin_PROGRAMS = audiofilter

audiofilterdir = src/

audiofilter_SOURCES = src/anypass.cpp src/delay.cpp src/fft_wrapper.cpp src/filter_make.cpp src/chorus.cpp src/io.cpp src/main.cpp src/pitch_shift.cpp src/window.cpp
audiofilter_HEADERS = src/effects.hpp src/fft_wrapper.hpp src/io.hpp src/window.hpp

AM_CFLAGS = -Wall- std=c++17

audiofilter_CPPFLAGS = -std=c++17
audiofilter_LDADD = -lfftw3 -lsndfile -lstdc++fs
