#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GuiDemo.h"
#include <opencv2/opencv.hpp>

#define BUFFING_LEVEL_MAX	0.07
#define BUFFING_LEVEL_MIN	0.0
#define WHITEN_LEVEL_MAX	1.0
#define WHITEN_LEVEL_MIN	0.0

class GuiDemo : public QMainWindow
{
	Q_OBJECT

public:
	GuiDemo(QWidget *parent = Q_NULLPTR);
	~GuiDemo();
private:
	bool initVideoCapture();
	void faceBeautyProcess();
	QImage mat2Image(cv::Mat& cvImg);

	void outputLog(QString cnt);
private slots:
	void nextFrame();
	void on_filterLevelSlider_valueChanged(int level);
	void on_whiteLevelSlider_valueChanged(int level);
private:
	Ui::GuiDemoClass ui;

	QImage mRawImage;
	QImage mBeautyImage;
	cv::Mat mRawFrame;
	cv::Mat mBeautyFrame;
	cv::VideoCapture videoCapture;

	QTimer *mTimer;
	double rate;

	float buffingLevel = 0.0f;
	float whiteLevel = 0.0f;

	float *interBuf = nullptr;
};
