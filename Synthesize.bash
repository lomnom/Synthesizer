#!/bin/bash
cpp () 
{ 
    g++ $1 -Og -g -pthread -std=c++2a -lpthread -o $2
}

cpp Synthesizer.cpp Shell && ./Shell $@ | out123 -e u8 -r 44100