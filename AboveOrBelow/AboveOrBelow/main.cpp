#include <string>
#include "Generator.h"
#include "DataReader.h"
#include <iostream>
#include "../DataVisualizer/paint.h"
#include "../NeuralNetwork/BackPropNeuralNet.h"
#include <future>

using namespace std;

void showVectorVals(string label, vector<double> &v)
{
	cout << label << " ";
	for (unsigned i = 0; i < v.size(); ++i) {
		cout << v[i] << " ";
	}

	cout << endl;
}

int Calculate(vector<unsigned> topology)
{
	auto handle = std::async(std::launch::async, visualizer::run);

	GenerateTrainData(3, topology);

	visualizer::DrawFunction(GeneratePoints(3));

	neuralnet::Net myNet(topology);

	DataReader trainData("./Data.txt");
	clock_t start;
	double duration;

	vector<double> inputVals, targetVals, resultVals;
	int trainingPass = 0;

	start = clock();
	while (!trainData.isEof()) {
		++trainingPass;
		cout << endl << "Pass " << trainingPass;

		// Get new input data and feed it forward:
		if (trainData.getNextInputs(inputVals) != topology[0]) {
			break;
		}

		showVectorVals(": Inputs:", inputVals);

		vector<double> temp = { inputVals.front() / AREA_X, inputVals.back() / AREA_Y };
		myNet.feedForward(temp);

		// Collect the net's actual output results:
		myNet.getResults(resultVals);
		showVectorVals("Outputs:", resultVals);

		// Train the net what the outputs should have been:
		trainData.getTargetOutputs(targetVals);
		showVectorVals("Targets:", targetVals);
		assert(targetVals.size() == topology.back());

		visualizer::AddPoint(visualizer::Point(inputVals.front(), inputVals.back()), round(resultVals.front()) == targetVals.front());

		myNet.backProp(targetVals);

		// Report how well the training is working, average over recent samples:
		cout << "Net recent average error: "
			<< myNet.getRecentAverageError() << endl;
	}
	duration = (clock() - start) / static_cast<double>(CLOCKS_PER_SEC);

	cout << endl << "Done in " << duration << " seconds." << endl;

	system("pause");
	return 0;
}

int wmain()
{
	vector<unsigned> topology1 = {2, 2, 1};
	vector<unsigned> topology2 = {2, 4, 4, 1};
	vector<unsigned> topology3 = {2, 8, 8, 6, 4, 1};

	Calculate(topology2);
	// Calculate(topology2);
	// Calculate(topology3);
	return 0;
}
