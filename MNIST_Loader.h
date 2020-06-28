#include <fstream>
#include <iostream>
#include "../Matrix/Matrix.h"

using namespace std;

class MNIST {

vector<Matrix> images;
vector<Matrix> labels;

uint32_t magic_number;  
uint32_t num_images;
uint32_t num_rows;
uint32_t num_cols;

uint32_t toLittleEndian(uint32_t intput) {
        const uint32_t end    = 0b00000000000000000000000011111111;
        const uint32_t third  = 0b00000000000000001111111100000000;
        const uint32_t fourth = 0b00000000111111110000000000000000;
        const uint32_t start  = 0b11111111000000000000000000000000;

        uint32_t output = 0;

        output  = (intput & end)    << 3*8;
        output |= (intput & third)  << 8;
        output |= (intput & fourth) >> 8;
        output |= (intput & start)  >> 3*8;



        return output;
}

public:
    bool readData(std::string filename) {
        ifstream mnist (filename, std::ifstream::binary | std::ifstream::in);

        if (!mnist.is_open())
            return false;

        magic_number = 0; 
        num_images   = 0;
        num_rows     = 0;
        num_cols     = 0;
    
        mnist.read(reinterpret_cast<char *>(&magic_number), sizeof(magic_number));
        mnist.read(reinterpret_cast<char *>(&num_images), sizeof(num_images));
        mnist.read(reinterpret_cast<char *>(&num_rows), sizeof(num_rows));
        mnist.read(reinterpret_cast<char *>(&num_cols), sizeof(num_cols));

        magic_number = toLittleEndian(magic_number);
        num_images   = toLittleEndian(num_images);
        num_rows     = toLittleEndian(num_rows);
        num_cols     = toLittleEndian(num_cols);                

        for (int i = 0; i < num_images; i++) {
            unsigned char * image = new unsigned char [num_rows * num_cols]; 
            mnist.read((char *)image, num_rows * num_cols);

            double * input = new double[num_rows * num_cols];

            for(int i = 0; i < num_rows * num_cols; i++) {
                input[i] = static_cast<double>(image[i]);
            }

            images.push_back(Matrix(input, num_rows, num_cols));

            delete [] input;
            delete [] image;
        }

        mnist.close();
        
        return true;

    }   
    
    bool readLabelData(string filename) {
        ifstream mnist (filename, std::ifstream::binary | std::ifstream::in);

        if (!mnist.is_open())
            return false;

        magic_number = 0; 
        num_images   = 0;
    
        mnist.read(reinterpret_cast<char *>(&magic_number), sizeof(magic_number));
        mnist.read(reinterpret_cast<char *>(&num_images), sizeof(num_images));


        magic_number = toLittleEndian(magic_number);
        num_images   = toLittleEndian(num_images);
                
        unsigned char * value = new unsigned char [num_images];

        mnist.read((char *)value, sizeof(value) * num_images);

        for (int i = 0; i < num_images; i++) {
            labels.push_back(Matrix({{double(static_cast<int>(value[i]))}}));
        }

        delete value;
        mnist.close();
        
        return true;
    }

    bool writePPM(int index) {
        Matrix image = images.at(index);

        ofstream output_ppm ("./image" + std::to_string(index) + "_isa_" + std::to_string(int(labels.at(index)(0))) + ".pgm", std::ofstream::binary | std::ofstream::out);
        
        if (!output_ppm.is_open())
            return false;

        output_ppm << "P5" << endl << num_rows << " " << num_cols << endl << "255" << endl; 

        const double * values = image.getValues();
        unsigned char * output_values = new unsigned char [image.size()];

        for(int i = 0; i< image.size(); i++) 
            output_values[i] = static_cast<unsigned char> (values[i]);

        output_ppm.write((char *) output_values, image.size());

        output_ppm.close();
        delete [] output_values;

        return true;
    }

    vector<Matrix> getFlattenedImages() const {
        vector<Matrix> output = images;
        for (auto & matrix : output) {
            matrix = matrix.flattenVertical();
        }
        return output;
    }

    inline
    vector<Matrix> getImages() const {
        return images;
    }

    inline
    vector<Matrix> getLabels() const {
        return labels;
    }
    
    inline
    int getNumImages() const {
        return num_images;
    }

    inline
    std::pair<int, int> getShape() const {
        return std::make_pair(num_rows, num_cols);
    }

    inline
    int getNumRows() const {
        return num_rows;
    }

    inline
    int getNumColumns() const {
        return num_cols;
    }

    inline
    int getMagicNumber() const {
        return magic_number;
    }
    




};





