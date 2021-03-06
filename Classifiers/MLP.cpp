#include "MLP.h"



MLP::MLP()
{
}


MLP::~MLP()
{
}

void MLP::train(cv::Mat& trainingData, cv::Mat trainingLabels)
{
	assert(trainingData.rows == trainingLabels.rows);
	cout << "\nWould you like to load MLP.xml? y/n\n";
	std::string input;
	cin >> input;
	if (input == "y") {
		mlp = StatModel::load<ANN_MLP>("MLP.xml");
	}
	if (mlp == NULL)
	{
		cout << "Starting training\n";
		cv::Mat layers = cv::Mat(4, 1, CV_32SC1);

		layers.row(0) = cv::Scalar(trainingData.cols);
		layers.row(1) = cv::Scalar(trainingData.cols);
		layers.row(2) = cv::Scalar(trainingData.cols);
		layers.row(3) = cv::Scalar(8);

		mlp = ml::ANN_MLP::create();
		TermCriteria criteria;
		criteria.maxCount = 100;
		criteria.epsilon = 0.00001f;
		criteria.type = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;
		mlp->setLayerSizes(layers);
		mlp->setTrainMethod(ml::ANN_MLP::BACKPROP);
		mlp->setBackpropWeightScale(0.05f);
		mlp->setBackpropMomentumScale(0.05f);
		mlp->setTermCriteria(criteria);
		mlp->setActivationFunction(ml::ANN_MLP::SIGMOID_SYM, 1, 1);
		// train
		Mat newlabels = Mat::zeros(trainingLabels.rows, 8, CV_32FC1);
		for (int i = 0; i < trainingLabels.rows; i++)
		{
			newlabels.at<float>(i, trainingLabels.at<float>(i, 0)) = 1.0;
		}

		mlp->train(ml::TrainData::create(trainingData, ml::SampleTypes::ROW_SAMPLE, newlabels));
		mlp->save("MLP.xml");
		cout << "Training finished\n";
	}
}

void MLP::predict(Mat &testData, Mat &testLabels,  Mat &result)
{
	assert(testData.rows == testLabels.rows);
	cv::Mat response(1, 8, CV_32FC1);
	cv::Mat predicted(testLabels.rows, 1, CV_32F);
	for (int i = 0; i < testData.rows; i++) {
		cv::Mat response(1, 8, CV_32FC1);
		cv::Mat sample = testData.row(i);

		mlp->predict(sample, response);

		int maxIndex = 0; float maxValue = 0;
		for (int j = 0; j < 8; j++)
		{
			if (response.at<float>(0, j) > maxValue)
			{
				maxIndex = j;
				maxValue = response.at<float>(0, j);
			}
		}
		predicted.at<float>(i, 0) = maxIndex;

	}

	cout << "Accuracy_{MLP} = " << evaluate(predicted, testLabels) << endl;
}

std::vector<float> MLP::predict(Mat &testData, float threshold)
{
	Mat probability = Mat::zeros(1, 8, CV_32FC1);
	std::vector<float> result;
	mlp->predict(testData, probability);

	for (int j = 0; j < 8; j++)
	{
		if(probability.at<float>(0, j) > threshold)
			result.push_back(j);
	}
	return result;
}