#include "GuiDemo.h"
#include <QTimer>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../FaceBeauty/EdgePreservingFilter.h"
#include "../FaceBeauty/skinWhiten.h"

GuiDemo::GuiDemo(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	if (initVideoCapture()) {
		rate = videoCapture.get(cv::CAP_PROP_FPS);
		rate = 15.0f;
		videoCapture >> mRawFrame;
		if (!mRawFrame.empty()) {
			int width = mRawFrame.cols;
			int height = mRawFrame.rows;
			int channel = mRawFrame.channels();
			const int width_height = width * height;
			const int width_channel = width * channel;
			const int width_height_channel = width * height * channel;
			interBuf = new float[(width_height_channel + width_height
								     + width_channel + width) * 2];


			mBeautyFrame = mRawFrame.clone();
			mRawImage = mat2Image(mRawFrame);
			ui.rawLabel->setPixmap(QPixmap::fromImage(mRawImage));
			mTimer = new QTimer(this);
			mTimer->setInterval(1000 / rate);
			connect(mTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
			mTimer->start();
		}
	}
}

GuiDemo::~GuiDemo()
{
	delete[] interBuf;
}

bool GuiDemo::initVideoCapture()
{
	videoCapture.open(0);
	
	bool ret = videoCapture.isOpened();

	return ret;
}

void GuiDemo::faceBeautyProcess()
{
	unsigned long beginTime, endTime;
	beginTime = clock();

	filter_by_rbf(mRawFrame, mBeautyFrame, buffingLevel, 0.1, interBuf);

	skinWhiten_brightness(mBeautyFrame, whiteLevel);

	endTime = clock();
	float elapsedTime = float((unsigned long)endTime - beginTime) / CLOCKS_PER_SEC;
	float fps = 1.0 / elapsedTime;
	int width = mRawFrame.cols;
	int height = mRawFrame.rows;
	QString outStr = QString("Image(%1*%2) -- %3ms -- %4fps").arg(width).arg(height).arg(elapsedTime*1000).arg(fps);
	outputLog(outStr);
}

QImage GuiDemo::mat2Image(cv::Mat& cvImg)
{
	QImage qImg;
	if (cvImg.channels() == 3)                             //3 channels color image
	{

		cv::cvtColor(cvImg, cvImg, CV_BGR2RGB);
		qImg = QImage((const unsigned char*)(cvImg.data),
			cvImg.cols, cvImg.rows,
			cvImg.cols*cvImg.channels(),
			QImage::Format_RGB888);
	}
	else if (cvImg.channels() == 1)                    //grayscale image
	{
		qImg = QImage((const unsigned char*)(cvImg.data),
			cvImg.cols, cvImg.rows,
			cvImg.cols*cvImg.channels(),
			QImage::Format_Indexed8);
	}
	else
	{
		qImg = QImage((const unsigned char*)(cvImg.data),
			cvImg.cols, cvImg.rows,
			cvImg.cols*cvImg.channels(),
			QImage::Format_RGB888);
	}

	return qImg;
}

void GuiDemo::outputLog(QString cnt)
{
	ui.textEdit->append(cnt);
}

void GuiDemo::nextFrame()
{

	videoCapture >> mRawFrame;

	if (!mRawFrame.empty()) {

		faceBeautyProcess();

		mRawImage = mat2Image(mRawFrame);
		ui.rawLabel->setPixmap(QPixmap::fromImage(mRawImage));
		mBeautyImage = mat2Image(mBeautyFrame);
		ui.beautyLabel->setPixmap(QPixmap::fromImage(mBeautyImage));
		update();
	}
}

void GuiDemo::on_filterLevelSlider_valueChanged(int level)
{
	buffingLevel = (BUFFING_LEVEL_MAX - BUFFING_LEVEL_MIN) / 100.0 * level;
	std::cout << "buffingLevel: " << buffingLevel << std::endl;
}

void GuiDemo::on_whiteLevelSlider_valueChanged(int level)
{
	whiteLevel = (WHITEN_LEVEL_MAX - WHITEN_LEVEL_MIN) / 100.0 * level;
	std::cout << "whiteLevel: " << whiteLevel << std::endl;
}
