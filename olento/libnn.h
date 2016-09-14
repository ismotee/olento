//
//  libnn.h
//  libnn
//
//  Created by Ismo Torvinen on 23.5.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#ifndef __libnn__libnn__
#define __libnn__libnn__

#include <stdio.h>
#include <vector>
#include <string>

struct linkData
{
    int source_layer;
    int source_id;
    int dest_layer;
    int dest_id;
    
    linkData(int s_layer, int s_id, int d_layer, int d_id):
        source_layer(s_layer),
        source_id(s_id),
        dest_layer(d_layer),
        dest_id(d_id)
    {}

};



class Neuron
{
public:
    
    Neuron();
    
    virtual void forward(void) = 0;
    virtual float back(void) = 0;
    virtual void setInput(float value) = 0;
    virtual void addLink(Neuron* neuron) = 0;
    
    float error;
    float output;
    std::vector<float> weights;  // bias + weights

    std::string toString();
    void fromString(std::string data);
    
protected:
    float learnRate;
};

struct NeuronData
{
    std::vector<float> weights;
    float output;
    NeuronData() {}
    NeuronData(Neuron& original) : output(original.output)
    {
        weights = original.weights;
    }
};


class HiddenNeuron : public Neuron
{
public:
    HiddenNeuron(float learn_rate = 0.004f);
    
    std::vector<Neuron*> upper;
    
    void forward(void);
    float back(void);
    void setInput(float value);
    void addLink(Neuron* neuron);
};

class InputNeuron : public Neuron
{
public:
    InputNeuron(float learn_rate = 0.001f);
    
    float input;
    
    void forward(void);
    float back(void);
    void setInput(float value);
    void addLink(Neuron* neuron);
    
};


class NLayer
{
public:
    NLayer();
    
    std::vector<Neuron*> neurons;
    
    void init(int neurons_n, bool is_input_layer = false);
    void forward();
    std::vector<float> back();
    void clearErrors();
    std::vector<NeuronData> getNeuronData();

    // for output layer;
    std::vector<float> getOutput();
    void setError(std::vector<float> errors);
    
    // for inputLayer
    void setInputs(std::vector<float> inputs);
    
    std::string toString();
    void fromString(std::string data);
    
};

struct NNetData
{
    int inputs_n,hidden_layers_n,hidden_neurons_n, outputs_n;
    
    std::vector<NeuronData> nData;
    std::vector<linkData> lData;
    std::string neurons_str;
    
    NNetData();
    NNetData(int in_n, int hl_n, int hn_n, int o_n) : inputs_n(in_n),hidden_layers_n(hl_n), hidden_neurons_n(hn_n),outputs_n(o_n)
    {
    }
    
    std::string toString();
    NNetData fromString(std::string str);
    
};

class NNet
{
public:
    NNet();
    
    void init(int inputs_n, int hidden_layers_n, int hidden_neurons_n, int outputs_n);
    
    std::vector<float> forward(std::vector<float> inputs);
    std::vector<float> back(std::vector<float> desired_output);
    void link(linkData);
    void link(int source_layer, int source_id, int dest_layer, int dest_id);
    void printSize();
    void saveNet(std::string filename);
    void loadNet(std::string filename);
    
    void setWeight(float value,int layer, int neuron, int weight);
    void setWeights(std::vector<NeuronData> nData);
    void setInputWeight(float value, int neuron, int weight);
    
protected:
    void linkInput(int input_id, int hidden_id);
    void linkHidden(int source_layer, int source_id, int dest_layer, int dest_id);
    NNetData nnData;
    NLayer inputLayer;
    std::vector<NLayer> hiddenLayers;
    NLayer outputLayer;
};


#endif /* defined(__libnn__libnn__) */
