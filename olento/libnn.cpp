//
//  libnn.cpp
//  libnn
//
//  Created by Ismo Torvinen on 23.5.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#include "libnn.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>

//SIGMOID
const float SIGMOID_RESPONSE = 0.5;


void Sigmoid(float* netinput)
{
    *netinput = ( 1 / ( 1 + exp(-(*netinput) / SIGMOID_RESPONSE)));
}

//----------------------------------------
// argumentiksi annetaan Sigmoid(x) => palauttaa Sigmoid’ (x)

float deltaSigmoid(float sig_x)
{
    return (sig_x ) * (1 - sig_x );
}

//-------------------------

Neuron::Neuron () :
    error(0),
    output(0)
{
}

void Neuron::fromString(std::string data) {
    
    std::stringstream ss;
    ss.str(data);
    
    float value;
    int weight_id = 0;
    ss >> output;
    while(ss >> value) {
        if(weight_id < weights.size()) {
            weights[weight_id] = value;
            weight_id++;
        }
        else {
            std::cout << "neuron: data too big: " << weight_id << "\n";
        }
    }
    
}

std::string Neuron::toString() {
    std::stringstream ss;
    ss << output << " ";
    for(int i = 0; i < weights.size();i++)
        ss << weights[i] << " ";

    ss << "\n";
    return ss.str();
}

HiddenNeuron::HiddenNeuron (float learn_rate)
{
    learnRate = learn_rate;
    weights.push_back(0.0f);
}

void HiddenNeuron::forward()
{
    output = 0;
    
    output += 1 * weights[0];  // bias
    
    for(int i =0;i < upper.size();i++)
    {
        output += upper[i]->output * weights[i+1]; //ohitetaan bias lisäämällä weights indeksiin +1
    }
    
    Sigmoid(&output);

}


float HiddenNeuron::back()
{

    //weights[0] = weights[0] + learnRate * error * 1; // bias
    
    for(int i = 1; i < upper.size(); i++)
    {
        upper[i]->error += deltaSigmoid (output) * error;
        weights[i+1] = weights[i+1] + learnRate * upper[i]->error * upper[i]->output;
    }
    return upper[0]->error;
    
}

void HiddenNeuron::setInput(float value)
{
    std::cout << "don't use this set input function!!!\n";
    upper[0]->output = value;
}

void HiddenNeuron::addLink(Neuron* neuron)
{
    upper.push_back(neuron);
    weights.push_back(0.0f);
}


InputNeuron::InputNeuron(float learn_rate) :
    input(0)
{
    learnRate = learn_rate;
    weights.push_back(1); // bias
    weights.push_back(0); // input
}

void InputNeuron::forward()
{
    output = 0;
    
    output += 1 * weights[0];  // bias
    output += input * weights[1]; // input
    
    Sigmoid(&output);
    
}

float InputNeuron::back()
{
    //weights[0] = weights[0] + learnRate * error * 1; // bias
    weights[1] = weights[1] + learnRate * error * input;
    return deltaSigmoid(output) + error;
}

void InputNeuron::setInput(float value)
{
    input = value;
}

void InputNeuron::addLink(Neuron* neuron)
{
    std::cout << "cannot link input layer with addLink command\n";
}
// end of Neuron














// NLayer

NLayer::NLayer ()
{
}

void NLayer::init(int neurons_n, bool is_input_layer)
{
    if(!is_input_layer)
        for (int i = 0; i < neurons_n; i++) neurons.push_back(new HiddenNeuron());
    else
        for (int i = 0; i < neurons_n; i++) neurons.push_back(new InputNeuron());
}


void NLayer::forward()
{
    for(int i = 0; i < neurons.size(); i++)
    {
        neurons[i]->forward();
    }
}

std::vector<float> NLayer::back()
{
    std::vector<float> result(neurons.size());
    for(int i = 0; i < neurons.size(); i++)
    {
        neurons[i]->back();
    }
    return result;
}

void NLayer::setInputs(std::vector<float> inputs)
{
    
    
    if(inputs.size() == neurons.size())
        for(int i = 0; i < neurons.size();i++)
            neurons[i]->setInput(inputs[i]);
    else
        std::cout << "wrong input vector size: "<< inputs.size() <<"\n";
}

std::vector<float> NLayer::getOutput()
{
    std::vector<float> result;
    
    for(int i = 0; i < neurons.size(); i++)
        result.push_back(neurons[i]->output);

    return result;
}

void NLayer::setError(std::vector<float> errors)
{
    
    if (errors.size() == neurons.size()) {
        for (int i = 0; i < neurons.size(); i++) {
            neurons[i]->error =
            //deltaSigmoid(neurons[i]->output) *
            (errors[i] - neurons[i]->output);
        }
    }
    else
    {
        std::cout << "wrong errors vector size!! (" << errors.size() << ")\n";
    }

}

void NLayer::clearErrors()
{
    for(int i = 0; i < neurons.size();i++)
    {
        neurons[i]->error = 0;
    }
}

std::vector<NeuronData> NLayer::getNeuronData ()
{
    std::vector<NeuronData> data;
    for(int i = 0; i < neurons.size(); i++)
        data.push_back(*neurons[i]);

    return data;
}

void NLayer::fromString(std::string data) {
    std::stringstream ss;
    ss.str(data);
    std::string line;
    
    int neurons_id = 0;
    
    while(ss.good()) {
        if(neurons_id < neurons.size()) {
            std::getline(ss, line);
            neurons[neurons_id]->fromString(line);
            neurons_id++;
        
        } else {
            std::cout << "NLayer: data too big!\n";
        }
    }
    
    
    
}

std::string NLayer::toString() {
    std::string result;
    for(int i = 0; i < neurons.size(); i++) {
        result += neurons[i]->toString();
    }
    return result;
}

// end of NLAYER

NNetData::NNetData(){}


std::string NNetData::toString ()
{
    std::stringstream ss;

    ss << inputs_n << " " << hidden_layers_n << " " << hidden_neurons_n << " " << outputs_n << "\n";

    for(int i = 0; i < lData.size();i++)
    {
        ss << "l" << "\n";
        ss << lData[i].source_layer << " " << lData[i].source_id << " " << lData[i].dest_layer << " " << lData[i].dest_id << "\n";
    }
            
    ss << "\n";
       
    return ss.str();
    
}

NNetData NNetData::fromString(std::string str)
{
    NNetData data;
    std::stringstream ss;
              
    ss.str(str);
    std::string the_line;
            
    std::getline(ss,the_line);
            
    std::stringstream values;
    values.str(the_line);
            
    // kirjoita init arvot
    
    values >> data.inputs_n;
    values >> data.hidden_layers_n;
    values >> data.hidden_neurons_n;
    values >> data.outputs_n;

    int neuron_id = 0;
    
    //aloitetaan lukemaan stringia


    
    while(!ss.eof() ){

        std::getline(ss, the_line);


        // l niin tehdaan linkki
        if(the_line.compare("l") == 0) {
            std::getline(ss, the_line);
            std::stringstream values;
            values.str(the_line);         

            int src_layer;
            int src_neuron;
            int dst_layer;
            int dst_neuron;


            
            values >> src_layer;
            values >> src_neuron;
            values >> dst_layer;
            values >> dst_neuron;
            
            std::cerr << "linkki: " << src_layer << " " << src_neuron << " " << dst_layer << " " << dst_neuron << "\n";
            
            linkData link(src_layer,src_neuron,dst_layer,dst_neuron);
            data.lData.push_back(link);
        }

        // jos n niin kirjoitetaan weight_str:aan
        if(the_line.compare("n") == 0) {
            float value;
            std::getline(ss, the_line);
            nData.push_back(NeuronData());
            
            values >> value;
            nData.back().output = value;
            
            while(values >> value) {
                nData.back().weights.push_back(value);
            }
                        
        }
        
    }
    
}





// NNET

NNet::NNet()
{
}

void NNet::init(int inputs_n, int hidden_layers_n, int hidden_neurons_n, int outputs_n)
{
    inputLayer.init(inputs_n, true);
    outputLayer.init(outputs_n);

    for(int i = 0; i < hidden_layers_n; i++) {
        hiddenLayers.push_back(NLayer());
        hiddenLayers.back().init(hidden_neurons_n);
    }
    
    // link all neurons from last hidden layer to every output layer's neuron.
    for(int i= 0; i < outputs_n; i++) {
        for(int j=0; j < hidden_neurons_n; j++) {
            outputLayer.neurons[i]->addLink(hiddenLayers.back().neurons[j]);
        }
    }
    
    nnData = NNetData(inputs_n, hidden_layers_n, hidden_neurons_n, outputs_n);
    
}


std::vector<float> NNet::forward(std::vector<float> inputs)
{
    inputLayer.setInputs(inputs);
    inputLayer.forward();
    
    for(int i = 0; i < hiddenLayers.size(); i++)
        hiddenLayers[i].forward();
    
    outputLayer.forward();
    
    return outputLayer.getOutput();
    
}

std::vector<float> NNet::back(std::vector<float> desired_output)
{
    
    // clear errors form every layer
    inputLayer.clearErrors();
    
    for (int i = 0; i < hiddenLayers.size(); i++) {
        hiddenLayers[i].clearErrors();
    }
    
    outputLayer.clearErrors();
    
    
    // set and calculate new errors for every layer
    outputLayer.setError(desired_output); // deal output layer's error separately
    outputLayer.back();
    
    for(int i = (int)hiddenLayers.size() - 1; i >= 0 ; i-- )
        hiddenLayers[i].back();
    
    return inputLayer.back();

}

void NNet::linkInput(int input_id,int hidden_id)
{
    hiddenLayers[0].neurons[hidden_id]->addLink(inputLayer.neurons[input_id]);
}

void NNet::linkHidden(int source_layer, int source_id, int dest_layer, int dest_id)
{
    
    if(hiddenLayers.size() > dest_layer &&
       hiddenLayers.size() > source_layer &&
       hiddenLayers[dest_layer].neurons.size() > dest_id &&
       hiddenLayers[source_layer].neurons.size() > source_id)
    {
        hiddenLayers[dest_layer].neurons[dest_id]->addLink(hiddenLayers[source_layer].neurons[source_id]);
    }
    else
    {
        std::cout << "bad link indexes!!!\n";
    }
}

void NNet::link(linkData data)
{
    if(data.source_layer == 0)
        linkInput(data.source_id, data.dest_id);
    else
        linkHidden(data.source_layer, data.source_id, data.dest_layer, data.dest_id);
    
    nnData.lData.push_back(data);
    
}

void NNet::link(int source_layer, int source_id, int dest_layer, int dest_id)
{
    linkData data(source_layer,source_id,dest_layer,dest_id);
    link(data);
}

void NNet::setWeights(std::vector<NeuronData> nData)
{
    int layer_id = 0;
    int neuron_decr = 0;
    
    for(int i = 0; i < nData.size(); i++) {
        for(int j = 0; j < nData[i].weights.size(); j++) {
            if(layer_id == 0) {
                inputLayer.neurons[i - neuron_decr]->weights[j] = nData[i].weights[j];
                inputLayer.neurons[i - neuron_decr]->output = nData[i].output;
                if(i == nnData.inputs_n) {
                    layer_id++;
                    neuron_decr += nnData.inputs_n;
                }
            } else if(1 + nnData.hidden_layers_n == layer_id) {
                outputLayer.neurons[i - neuron_decr]->weights[j] = nData[i].weights[j];
                outputLayer.neurons[i - neuron_decr]->output = nData[i].output;
            } else {
                hiddenLayers[layer_id -1].neurons[i - neuron_decr]->weights[j] = nData[i].weights[j];
                hiddenLayers[layer_id -1].neurons[i - neuron_decr]->output = nData[i].output;
                if(i - neuron_decr == nnData.hidden_neurons_n) {
                    layer_id++;
                    neuron_decr += nnData.hidden_neurons_n;
                }
            }
        }
    }
}

void NNet::setWeight(float value, int layer, int neuron, int weight)
{
    hiddenLayers[layer].neurons[neuron]->weights[weight] = value;    
}

void NNet::setInputWeight(float value, int neuron, int weight) {
    inputLayer.neurons[neuron]->weights[weight] = value;
}


void NNet::saveNet(std::string filename)
{
    std::ofstream os(filename);
    
    if(os.is_open()) {

        std::string data_str;
        
        //perustiedot kuten inputtien määrä, linkit, jne..
        data_str = nnData.toString();
        
        data_str += " L 0 " + std::to_string(nnData.inputs_n) + "\n"; 
        data_str += inputLayer.toString();
 
        for(int i = 0; i < hiddenLayers.size();i++) {
            data_str += " L " + std::to_string(i + 1) + " " + std::to_string(nnData.hidden_neurons_n) + "\n";
            data_str += hiddenLayers[i].toString();
        }
        
        //data_str += "L " + std::to_string(nnData.hidden_layers_n + 1) + " " + std::to_string(nnData.outputs_n) + "\n"; 
        //data_str += outputLayer.toString();
               
        os << data_str;
        os.close();

    } else
        std::cout << "no save file open";
    
}

void NNet::loadNet(std::string filename) 
{
    std::ifstream is(filename);

    if(is.is_open()) {
        std::string line;
        std::stringstream ss;

        std::getline(is,line);
        ss.str(line);
        
        int in_n;
        int hl_n;
        int hn_n;
        int o_n;
        
        ss >> in_n;
        ss >> hl_n;
        ss >> hn_n;
        ss >> o_n;
        
        std::cout << "ladataan: " << in_n << " "<< hl_n << " " << hn_n << " " << o_n << "\n";
        
        init(in_n,hl_n,hn_n,o_n);
        
        while(is.good()) {
            std::stringstream loopstream;
            // reading id_letter and possibly other data to stream
            std::getline(is,line);
            
            loopstream.str(line);

            std::string id_letter;
            loopstream >> id_letter;
            
            //std::cout <<id_letter<<"\n";
            // link
            if(id_letter.compare("l") == 0) {
                // get linking data from file
                std::getline(is,line);
                std::stringstream values;
                values.str(line);         

                // convert string to int
                int src_layer;
                int src_neuron;
                int dst_layer;
                int dst_neuron;
                
                values >> src_layer;
                values >> src_neuron;
                values >> dst_layer;
                values >> dst_neuron;
                
                //std::cout << "tehdään linkki: " << src_layer << " " << src_neuron << " " << dst_layer << " " << dst_neuron << "\n";
                //make a link
                link(src_layer, src_neuron, dst_layer, dst_neuron);
                
            }
            
            if(id_letter.compare("L") == 0) {
                int layer_id;
                int neurons_n;
                
                //loopstream.str(line);
                
                loopstream >> layer_id;
                loopstream >> neurons_n;
                
                std::cout << "Layer " << layer_id << ", " << neurons_n << " neuronia\n";
                
                std::string layerData;
                
                for(int i = 0; i < neurons_n - 1;i++) {
                    std::getline(is, line);
                    layerData += line + " \n";
                }
                
                
                if(layer_id == 0)
                    inputLayer.fromString(layerData);
                else if(layer_id == nnData.hidden_layers_n + 1)
                    outputLayer.fromString(layerData);
                else
                    hiddenLayers[layer_id-1].fromString(layerData);
                
            }
        
        }
    is.close();    
    }
}

void NNet::printSize()
{
    std::cout << "input Layer: " << inputLayer.neurons.size() <<
    " hidden layer: " << hiddenLayers.size() << " hidden layer neurons: " << hiddenLayers[0].neurons.size() << " output layer:" << outputLayer.neurons.size() << "\n";

}
