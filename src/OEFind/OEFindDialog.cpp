#include "stdafx.h"
#include "OEFindDialog.h"

OEFindDialog::OEFindDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(QStringLiteral("OE号查找"));
	setWindowFlags(Qt::WindowCloseButtonHint);
	setFixedSize(840,410); 


	QFont font;
	font.setPointSize(12);
	setFont(font);

	oe_label = new QLabel(QStringLiteral("OE号："));             
	brand_choice_label = new QLabel(QStringLiteral("品牌选择："));
	oe_find_button = new QPushButton(QStringLiteral("查找"));  
	connect(oe_find_button,SIGNAL(clicked()),this,SLOT(oe_find_slots()));
	oe_find_button->setEnabled(false);

	oe_find_edit = new QLineEdit;    
	oe_label->setBuddy(oe_find_edit);
	oe_find_edit->setPlaceholderText(QStringLiteral("请在此输入OE号，并选择品牌......"));
	connect(oe_find_edit, SIGNAL(textChanged(const QString &)),this, SLOT(oe_find_edit_slots()));

	brand_choice_combobox = new QComboBox();

	match_checkbox = new QCheckBox(QStringLiteral("完全匹配"));
	match_checkbox->setCheckState(Qt::Unchecked);

	all_brand_checkbox=new QCheckBox(QStringLiteral("全部车型"));
	all_brand_checkbox->setCheckState(Qt::Unchecked);
	connect(all_brand_checkbox,SIGNAL(stateChanged(int)),this,SLOT(oe_find_all_brand_slots()));

	find_result_tablewidget = new QTableWidget(10,4);
	QStringList header;
	header<<QStringLiteral("原厂件编码")<<QStringLiteral("品牌")<<QStringLiteral("原厂件名称")<<QStringLiteral("销售价格");
	find_result_tablewidget->setHorizontalHeaderLabels(header);
	find_result_tablewidget->setWindowTitle(QStringLiteral("查询结果"));
	find_result_tablewidget->setColumnWidth(0,240);
	find_result_tablewidget->setColumnWidth(1,70);
	find_result_tablewidget->setColumnWidth(2,240);
	find_result_tablewidget->setColumnWidth(3,240);


	find_result_tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);// 只读
	find_result_tablewidget->setSelectionMode(QAbstractItemView::ExtendedSelection); //可多选
	find_result_tablewidget->setFont(font);
	font.setBold(true);
	find_result_tablewidget->horizontalHeader()->setFont(font);

	QHBoxLayout *topLayout = new QHBoxLayout;
	topLayout->addWidget(oe_label);
	topLayout->addWidget(oe_find_edit);
	topLayout->addWidget(oe_find_button);

	QHBoxLayout *centreLayout = new QHBoxLayout;
	centreLayout->addWidget(brand_choice_label);
	centreLayout->addWidget(brand_choice_combobox);
	centreLayout->addStretch();
	centreLayout->addWidget(all_brand_checkbox);
	centreLayout->addWidget(match_checkbox);

	QVBoxLayout *mainLayout =new QVBoxLayout;
	mainLayout->addLayout(topLayout);
	mainLayout->addLayout(centreLayout);
	mainLayout->addWidget(find_result_tablewidget);
	setLayout(mainLayout);

	OE_conf_not_found.setWindowTitle(QStringLiteral("错误"));
	OE_conf_not_found.setIcon(QMessageBox::Critical);


	OE_not_found.setWindowTitle(QStringLiteral("查找结果"));
	OE_not_found.setIcon(QMessageBox::Warning);
	OE_not_found.setText(QStringLiteral("对不起，找不到该OE号，请核对后再查！"));

	OE_conf_parse_failed.setWindowTitle(QStringLiteral("错误"));
	OE_conf_parse_failed.setIcon(QMessageBox::Critical);
	OE_conf_parse_failed.setText(QStringLiteral("配置文件解析失败，应用程序错误，点击确认退出！"));

	OE_conf_parse_status.setWindowTitle(QStringLiteral("提示"));
	OE_conf_parse_status.setIcon(QMessageBox::Information);
	OE_conf_parse_status.setText(QStringLiteral("配置文件加载中，请稍后..."));
}

OEFindDialog::~OEFindDialog()
{

}
void OEFindDialog::oe_find_slots()
{
	QString find_edit_input = oe_find_edit->text();
	int brand_choice = brand_choice_combobox->currentIndex();
	bool all_brand = all_brand_checkbox->checkState();
	bool oe_march = match_checkbox->checkState();

	QByteArray ba = find_edit_input.toLatin1(); //转换成 char *
	char *find_edit_string = ba.data();
	trim_space(find_edit_string);
	strupr(find_edit_string);//转换成大写

	find_result_tablewidget->setRowCount(10);
	find_result_tablewidget->clearContents(); //刷新显示
	oe_find_edit->selectAll();

	if(OE_find.run(find_edit_string,brand_choice,all_brand,oe_march)) //转换
	{
		if(OE_find.OEData_find_result.size()>10)
			find_result_tablewidget->setRowCount(find_result_tablewidget->rowCount()+OE_find.OEData_find_result.size()-10);

		for(size_t i=0;i<OE_find.OEData_find_result.size();i++)
		{
			QString brand_string=brand_string.fromLocal8Bit(OE_find.OEData_find_result[i]->brand);
			QString oe_name_string =oe_name_string.fromLocal8Bit(OE_find.OEData_find_result[i]->oe_name);
			QString price_string =QString::number(OE_find.OEData_find_result[i]->price,0,6);

			QTableWidgetItem *oe_code=new QTableWidgetItem(OE_find.OEData_find_result[i]->oe_code);
			QTableWidgetItem *brand=new QTableWidgetItem(brand_string);
			QTableWidgetItem *oe_name=new QTableWidgetItem(oe_name_string);
			QTableWidgetItem *price=new QTableWidgetItem(price_string);

			find_result_tablewidget->setItem(i,0,oe_code);
			find_result_tablewidget->setItem(i,1,brand);
			find_result_tablewidget->setItem(i,2,oe_name);
			find_result_tablewidget->setItem(i,3,price);
		}

		OE_find.OEData_find_result.clear(); //清除查询结果
	}
	else
	{
		if(OE_not_found.exec()== QMessageBox::Ok)
		{
			oe_find_edit->setFocus();
			oe_find_edit->selectAll();
		}
	}
}
void OEFindDialog::oe_find_edit_slots()
{
	QString oe_find_edit_text = oe_find_edit->text();
	if(!oe_find_edit_text.isEmpty())
		oe_find_button->setEnabled(true);
	else
		oe_find_button->setEnabled(false);
}
void OEFindDialog::oe_find_all_brand_slots()
{
	if(all_brand_checkbox->checkState())
		brand_choice_combobox->setEnabled(false);
	else
		brand_choice_combobox->setEnabled(true);
}
bool OEFindDialog::init_conf_file()
{
	bool Message_error=false;

	if	(!OE_find.get_conf_dir()) //找不到初始化文件
	{
		OE_conf_not_found.setText(QStringLiteral("初始化文件OEFind.ini解析失败,请确认！"));
		OE_conf_not_found.exec();
		Message_error=true;
	}
/*	if	(!OE_find.check_config_Vehicle())//找不到Vehicle文件
	{
		OE_conf_not_found.setText(QStringLiteral("找不到配置文件：conf1，请确认！"));
		OE_conf_not_found.exec();
		Message_error=true;
	}
	if	(!OE_find.check_config_OEData())//找不到OEData文件
	{
		OE_conf_not_found.setText(QStringLiteral("找不到配置文件：conf2，请确认！"));
		OE_conf_not_found.exec();
		Message_error=true;
	}
	if	(!OE_find.check_config_BaseData())//找不到BaseData文件
	{
		OE_conf_not_found.setText(QStringLiteral("找不到配置文件：conf3，请确认！"));
		OE_conf_not_found.exec();
		Message_error=true;
	}*/
	
	if(Message_error)
		return false;
	else
		return true;
}
bool OEFindDialog::parse_conf_file()
{	
	QDialogButtonBox *child_button = OE_conf_parse_status.findChild<QDialogButtonBox*>();
	child_button->hide();
	OE_conf_parse_status.show();

	QCoreApplication::processEvents();//刷新显示

/*	if(!OE_find.file_encryption())  //配置文件加密
	{
		OE_conf_not_found.setText(QStringLiteral("配置文件错误,请确认！"));
		OE_conf_not_found.exec();
		return false;
	}*/
	
	if(!OE_find.parse_database())  
	{
		if(OE_conf_parse_failed.exec() == QMessageBox::Ok)
			return false;
	}
	OE_conf_parse_status.close();
	return true;
}
void OEFindDialog::init_brand_choice_combobox()
{
	DDETable *Brand_tb = OE_find.db_read.get("Brand");

	for(size_t i=0;i<Brand_tb->size();i++)
	{
		Brand *data = reinterpret_cast<Brand*>(Brand_tb->at(i));
		QString brand_string=brand_string.fromLocal8Bit(data->brand_name_cn);
		brand_choice_combobox->addItem(brand_string);
	}
}
