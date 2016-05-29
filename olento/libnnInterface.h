//
//  libnnInterface.h
//  libnn
//
//  Created by Ismo Torvinen on 29.5.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#ifndef __libnn__libnnInterface__
#define __libnn__libnnInterface__

#include <stdio.h>
#include "libnn.h"
#include <vector>
#include <atomic>

/*
// muuttujat ja luokat


 NNet nn_net;
 
 std::vector<float> nn_output;
 std::vector<float> nn_desired_out;
 std::vector<float> nn_input;
 
 std::atomic<bool> outRead;
 std::atomic<bool> inWritten;
 std::atomic<bool> desiredWritten;
 std::atomic<bool> nn_stop;

//funktiot:
*/
void Init();

void StartRoutine();

 // inWritten == true --> forward && outRead true
 // desiredWritten == true --> back && desiredWritten false
 // stop == true --> lopetetaan looppi

void SetInput(std::vector<float> input_);

    // syötä input nettiin
    // inWritten = true

std::vector<float> GetOutput();

    // outRead == true --> return empty.vector
    // ouRead == false --> return net.output

void SetDesiredOut(std::vector<float> desired_out_);

    // net.desired_out = desired_out
    // desiredWritten = true

void Close();

#endif /* defined(__libnn__libnnInterface__) */
