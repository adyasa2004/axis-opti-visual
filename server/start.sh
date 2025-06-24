#!/bin/bash
g++ -O2 -std=c++17 code.cpp -o axis_optimizer
chmod +x axis_optimizer
python app.py
