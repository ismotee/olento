//
//  libnnInterface.cpp
//  libnn
//
//  Created by Ismo Torvinen on 29.5.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#include "libnnInterface.h"
#include <iostream>
#include <mutex>

NNet nn_net;

std::vector<float> nn_output;
std::vector<float> nn_desired_out;
std::vector<float> nn_input;

std::atomic<bool> outRead;
std::atomic<bool> inWritten;
std::atomic<bool> desiredWritten;
std::atomic<bool> nn_stop;
std::atomic<bool> update;

std::recursive_mutex mtx;

void Init()
{
    // count num_inputs && num_hidden_neurons dynamically
    int in = 4;
    int hid = 0;
    for(int i = in; i > 0;i--)
        hid += i;
    
    nn_net.init(in, 1, hid, 8);
    
    int reduction =0;
    
    for(int i=0; i<in; i++) {
        reduction += i;
        for(int j=0; j<in; j++) {
            nn_net.link(0,i,1,i*in + j - reduction);
            nn_net.link(0,j,1,i*in + j - reduction);
        }
    }

}

void StartRoutine() {
    
        // aloita looppi
        nn_stop = false;
        while(!nn_stop) {
            // back propagation
            
            if(mtx.try_lock()){
                if(desiredWritten == true) {
                    nn_net.back(nn_desired_out);
                    desiredWritten = false;
                }


                // feed forward
                if(inWritten == true) {
                    nn_output = nn_net.forward(nn_input);
                    outRead = false;
                }
                mtx.unlock();
            }
               
            
            
        }
    }
    
void SetInput(std::vector<float> input_)
    {
        if(mtx.try_lock()) {
            nn_input = input_;
            inWritten = true;
            mtx.unlock();
        }
    }
    
std::vector<float> GetOutput()
    {
        std::vector<float> result;
        if(outRead) return std::vector<float>();
            if(mtx.try_lock()) {
                std::cout << "teach\n";
                outRead = true;
                result = nn_output;
                mtx.unlock();
            }
        return result;

    }

void SetDesiredOut(std::vector<float> desired_out_)
    {
        if(mtx.try_lock()) {
            nn_desired_out = desired_out_;
            desiredWritten = true;
            mtx.unlock();
        }
    }


void Close() {
	nn_stop = true;
}