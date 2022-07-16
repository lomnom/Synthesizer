#!/bin/bash
cpp () 
{ 
    g++ $1 -Og -g -pthread -std=c++2a -lpthread -o $2
}

cpp Generate.cpp Generate && ./Generate | out123 -e u8 -r 11025