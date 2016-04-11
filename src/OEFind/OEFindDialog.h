#ifndef OEFIND_H
#define OEFIND_H
#include "OEFind.h"
#include <QtWidgets/QDialog>
#include <QLabel.h>
#include <QPushButton.h>
#include <QLineEdit.h>
#include <QCheckBox.h>
#include <QComboBox.h>
#include <QBoxLayout.h>
#include <QTableWidget.h>

class OEFindDialog : public QDialog
{
	Q_OBJECT

public:
	OEFindDialog(QWidget *parent = 0);
	~OEFindDialog();

	bool init_conf_file();
	bool parse_conf_file();
	void init_brand_choice_combobox();

private slots:
	void oe_find_slots();
	void oe_find_edit_slots();
	void oe_find_all_brand_slots();
private:
	QLabel *oe_label;
	QLabel *brand_choice_label;
	QLineEdit *oe_find_edit;
	QPushButton *oe_find_button;
	QComboBox *brand_choice_combobox;
	QCheckBox *match_checkbox;
	QCheckBox *all_brand_checkbox;
	QTableWidget *find_result_tablewidget;
	QMessageBox OE_not_found;
	QMessageBox OE_conf_not_found;
	QMessageBox OE_conf_parse_failed;
	QMessageBox OE_conf_parse_status;
	OEFind OE_find;
};

#endif // OEFIND_H
