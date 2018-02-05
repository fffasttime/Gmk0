
#include "nn_cpp.h"

int main() {
	NN test(std::string("./weights3b32f.txt"));
	Network::NNPlanes input;
	input.resize(INPUT_CHANNELS);
	auto output = test.forward(input);
	std::cout << output.first.size() << std::endl;
	std::system("pause");
	return 0;
}

void testforward()
{

}
