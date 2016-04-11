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

	if(!MyDialog->init_conf_file())  //��ʼ�������ļ�
		return false;
	if(!MyDialog->parse_conf_file())  //���������ļ�
		return false;

	MyDialog->init_brand_choice_combobox();
	return app.exec();

}
