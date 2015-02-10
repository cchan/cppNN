#include <iostream>
#include <string>
#include <vector>
#include "../common.h"
#include <cassert>
#include "../LayeredNeural_Backprop_Floats/NeuralNetwork.h"
#include "../LayeredNeural_Backprop_Floats/Vectors.h"
using namespace std;



class ConvolutionalNeuralNetwork{//Associates smaller regions of data into meaningful output.
	//Sort of taking every pair of adjacent pixels to a neuron, except more complex.
	size_t inputSize;
	vector<size_t> sizes;
	NeuralNetwork combiner;
	std::vector<std::pair<NeuralNetwork, std::vector<size_t> > > networksIndices; //Pairs of NeuralNetworks and the corresponding sections of the input they take in.

	//some way to mutate the index assignments
	ConvolutionalNeuralNetwork(const size_t is, const std::vector<size_t>& sizes_inner, const std::vector<size_t>& sizes_combiner, vector<vector<size_t> > indicesSets){
		for (vector<size_t> indices : indicesSets){
			for (size_t ind : indices)
				assert(ind < is && ind >= 0);//I suppose you can have repeats, but I don't see why you would want them.
			networksIndices.push_back(make_pair(NeuralNetwork(indices.size(), sizes_inner), indices));
		}
		for (auto ni : networksIndices){
			ni.first.randInit();
		}
		inputSize = is;
		combiner = NeuralNetwork(networksIndices.size() * sizes_inner[sizes_inner.size() - 1], sizes_combiner); //It's accepting the input of networksIndices.size() number of nets, each last layer sized s.size()-1
	}
	vector<double> frontprop(vector<double> input){
		assert(input.size() == inputSize);

		vector<double> combinerInput;

		for (auto ni : networksIndices){
			vector<double> output_inner = ni.first.frontprop(extractIndices(input, ni.second));
			combinerInput.insert(combinerInput.end(), output_inner.begin(), output_inner.end());
		}

		return combiner.frontprop(combinerInput);
	}
};



class Image{
	friend ostream& operator<<(ostream& out, const Image& image);
private:
	const double max = 1.0;
	const double min = -1.0;
	string characterRamp = "x#"; //" .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"; //http://paulbourke.net/dataformats/asciiart/

	size_t width;
	size_t height;
	vector<double> image;

public:
	Image(size_t _height, size_t _width){
		assert(max > min);

		width = _width;
		height = _height;

		image.resize(width * height);
		for (size_t i = 0; i < width*height; i++)
			image[i] = rand11() > 0 ? 1 : -1;
			//image[i] = (rand11() + (max + min) / 2) * (max - min) / 2;
	}
	Image(size_t _height, size_t _width, vector<double> _image){
		assert(max > min);

		width = _width;
		height = _height;

		assert(_image.size() == width * height);
		for (double d : _image)
			assert(d <= max && d >= min);
		image = _image;
	}
	size_t getWidth() const{ return width; }
	size_t getHeight() const{ return height; }
	vector<double> raw() const{ return image; }
	void setPixelAt(size_t row, size_t col, double val){
		assert(row < height && row >= 0 && col < width && col >= 0 && val >= min && val <= max);
		image[width*row + col] = val;
	}
	double pixelAt(size_t row, size_t col) const{
		assert(row < height && row >= 0 && col < width && col >= 0);
		return image[width*row + col];
	}
	Image slice(vector<size_t> rows_to_keep, vector<size_t> cols_to_keep) const{
		vector<double> new_image(rows_to_keep.size() * cols_to_keep.size());
		size_t x = 0;
		for (size_t i : rows_to_keep){
			for (size_t j : cols_to_keep){
				new_image[x] = pixelAt(i, j);
				x++;
			}
		}
		return Image(rows_to_keep.size(), cols_to_keep.size(), new_image);
	}
};

ostream& operator<<(ostream& out, const Image& image){
	for (size_t i = 0; i < image.height; i++){
		for (size_t j = 0; j < image.width; j++){
			double val = image.pixelAt(i, j);
			size_t index = (size_t)floor((val - image.min) / (image.max - image.min) * image.characterRamp.length());
			out << image.characterRamp[index];
		}
		out << "\n";
	}
	return out;
}



int main(){//Classifier, whether it's a '0' - is there a way it can generate hypotheses?
	const int height = 3;
	const int width = 3;
	const int generations = 1000;

	NeuralNetwork nn(height*width, { 1, 1 }, 1.0001);

	for (int repeat = 0; repeat < 100; repeat++){
		string answer;
		int fails = 0;

		nn.randInit();

		for (int generation = 0; generation < generations; generation++){
			Image image(height, width);
			//cout << image;
			//cout << "Is this a '0'? (y/n) ";
			/*cin >> answer;
			while (answer[0] != 'y' && answer[0] != 'n'){
			cout << "(y/n) ";
			cin >> answer;
			}*/
			int count = 0;
			for (double val : image.raw()){
				if (val > 0) count++;
				else count--;
			}
			answer = count > 0 ? 'n' : 'y';
			//cout << answer << endl;

			vector<double> result;
			if (answer[0] == 'y')
				result = nn.backprop(image.raw(), { 2 });
			else
				result = nn.backprop(image.raw(), { -2 });
			assert(result.size() == 1);

			if (result[0] > 0 && count > 0 || result[0] <= 0 && count < 0 /*&& generation >= 1000*/){
				fails++;
				//cout << (++fails) << "/" << generation << ": " << answer << " - " << (result[0] > 0 ? 'y' : 'n') << " (" << result[0] << ")" << endl;
			}
		}
		cout << fails << "/" << generations /* - 1000*/ << endl;
	}
	cin.get();
}

