#include "stdafx.h"
#include "OEFindDialog.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	OEFindDialog *MyDialog;
	MyDialog = new OEFindDialog;

	QTranslator translator; 
	translator.load(":/qt_zh_CN"); 
	app.installTranslator(&translator); 

	MyDialog->show();

	if(!MyDialog->init_conf_file())  //初始化配置文件
		return false;
	if(!MyDialog->parse_conf_file())  //解析配置文件
		return false;

	MyDialog->init_brand_choice_combobox();
	return app.exec();

}
