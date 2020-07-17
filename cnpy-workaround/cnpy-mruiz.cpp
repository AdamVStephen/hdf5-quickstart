#include"cnpy.h"
#include<complex>
#include<cstdlib>
#include<iostream>
#include<map>
#include<string>

#ifdef MRUIZ
void mruiz_demo () {
cnpy::NpyArray signal = cnpy::npy_load("./Signal11078.npy");

int nsignal = signal.num_vals;

//cnpy::NpyArray cnn1bias = cnpy::npy_load("./savedfiles0/coeftxt/CNN1bias.npy");

cnpy::NpyArray cnn1bias = cnpy::npy_load("./CNN1bias.npy");

int nbias1 = cnn1bias.num_vals;

//cnpy::NpyArray cnn1weights = cnpy::npy_load("./savedfiles0/coeftxt/CNN1weights.npy");

cnpy::NpyArray cnn1weights = cnpy::npy_load("./CNN1weights.npy");

//float *weights1 = arr.data<float>();

int nweights1 = cnn1weights.num_vals;

//CNN1weights is organised as follows:

//(20,1,20) -> N filters output, 1 input signal and M kernel filter coefficients

// N values k0

// N values k1

// N values k2

// N values k sub M-1

// The organisation needs to be changed by

// First filter: k0,k1-> ksub M-1

// Second filter: k0,k1->k sub M-1

float *weights1_t = (float *)malloc(sizeof(float) * nweights1);

for (int filter = 0; filter < NCNN1; filter++)
{

    for (int k = 0; k < KCNN1; k++)
    {

        weights1_t[filter * KCNN1 + k] = cnn1weights.data<float>()[k * KCNN1 + filter];
    }
}

cnpy::NpyArray cnn2bias = cnpy::npy_load("./CNN2bias.npy");

int nbias2 = cnn2bias.num_vals;

cnpy::NpyArray cnn2weights = cnpy::npy_load("./CNN2weights.npy");

int nweights2 = cnn2weights.num_vals;

auto binaryFile = argv[1];

//Allocate Memory in Host Memory

cl_int err;

cl::Context context;

cl::CommandQueue q;

cl::Kernel krnl_adder;

size_t vector_size_bytes = sizeof(float) * DATA_SIZE;

std::vector<float, aligned_allocator<float>> source_input(DATA_SIZE);

std::vector<float, aligned_allocator<float>> source_weights(DATA_SIZE);

std::vector<float, aligned_allocator<float>> source_hw_results(DATA_SIZE);

std::vector<float, aligned_allocator<float>> source_sw_results(DATA_SIZE);
}
#endif 
// /* comment out the entire function for now */

int main(int argc, char *arg[])
{
    std::cout << "Buenas Dias : cnpy demo" << std::endl;
    return 0;
}