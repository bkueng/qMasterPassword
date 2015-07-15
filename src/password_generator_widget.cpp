#include "include/password_generator_widget.h"
#include "ui_password_generator_widget.h"

#include "crypto_functions.h"
#include "crypto.h"
#include "logging.h"
#include <QMessageBox>
#include <QPainter>
#include <QWhatsThis>

PasswordGeneratorWidget::PasswordGeneratorWidget(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PasswordGeneratorWidget)
{
	ui->setupUi(this);
	ui->lblEntropy->installEventFilter(this);

	uiChanged();

	//improve size behavior
	ui->lblStrength->setText("a\na"); //resize to have at least 2 lines
	layout()->activate();
	ui->lblStrength->setMinimumHeight(ui->lblStrength->height());
	QSize window_size = size();
	window_size.setHeight(minimumHeight());
	resize(window_size);
	ui->cmbMethod->setCurrentIndex(1);
	uiChanged();
}

PasswordGeneratorWidget::~PasswordGeneratorWidget()
{
	delete ui;
}

void PasswordGeneratorWidget::uiChanged() {
	int num_chars;
	double entropy_bits;

	ui->txtPassword->clear();

	switch (selectedMethod()) {
	case Method::Characters:
		num_chars = selectedCharacters().length();
		ui->btnGenerate->setEnabled(num_chars > 0);
		ui->templatesWidget->setVisible(false);
		ui->charactersWidget->setVisible(true);
		entropy_bits = (double)ui->txtPasswordLength->value() * log2((double)num_chars);
		break;
	case Method::Template:
	{
		ui->btnGenerate->setEnabled(true);
		ui->charactersWidget->setVisible(false);
		ui->templatesWidget->setVisible(true);
		vector<QString> templates;
		generateTemplates(templates, ui->txtPasswordLengthTemplate->value(),
				ui->chkTemplateSpecial->isChecked() ? 1 : 0,
				ui->chkTemplateNumber->isChecked() ? 1 : 0);
		entropy_bits = 0;
		QString chars;
		//Note: we assume each template contains only passwords that are in no
		//other of the templates
		LOG(DEBUG, "Number of pw templates: %i", (int)templates.size());
		for (int j = 0; j < (int)templates.size(); ++j) {
			double num = 1;
			for (int i = 0; i < templates[j].length(); ++i) {
				chars = QString::fromStdString(MasterPassword::charactersFromClass(
								templates[j][i].toLatin1()));
				num *= chars.length();
			}
			entropy_bits += num;
		}
		entropy_bits = log2(entropy_bits);
	}
		break;
	}
	if (entropy_bits < 0.) entropy_bits = 0.;
	m_current_entrypy_bits = entropy_bits;
	ui->lblEntropy->setText(QString::number((int)entropy_bits) + tr(" Bits"));
	ui->lblEntropy->repaint();

	//table source: http://rumkin.com/tools/password/passchk.php
	if (entropy_bits < 28.) {
		ui->lblStrength->setText(tr("Very Weak; might keep out family members"));
	} else if (entropy_bits < 36.) {
		ui->lblStrength->setText(tr("Weak; should keep out most people, often good for desktop login passwords"));
	} else if (entropy_bits < 60.) {
		ui->lblStrength->setText(tr("Reasonable; fairly secure passwords for network and company passwords"));
	} else if (entropy_bits < 128.) {
		ui->lblStrength->setText(tr("Strong; can be good for guarding financial information"));
	} else {
		ui->lblStrength->setText(tr("Very Strong; often overkill"));
	}
}

bool PasswordGeneratorWidget::eventFilter(QObject *obj, QEvent *event) {
	if (obj == ui->lblEntropy) {
		if (event->type() == QEvent::Paint) {
			QPainter painter(static_cast<QWidget *>(obj));
			painter.setRenderHint(QPainter::Antialiasing);

			QRect rect = ui->lblEntropy->rect();
			QLinearGradient gradient(rect.topLeft(), rect.topRight());
			gradient.setColorAt(0, Qt::red);
			gradient.setColorAt(0.25, Qt::yellow);
			gradient.setColorAt(0.8, Qt::green);
			gradient.setColorAt(1., Qt::green);

			rect.setWidth(rect.width()*m_current_entrypy_bits/128);
			painter.fillRect(rect, gradient);
		}
		return QDialog::eventFilter(obj, event);
	} else {
		// pass the event on to the parent class
		return QDialog::eventFilter(obj, event);
	}
}

QString PasswordGeneratorWidget::selectedCharacters() {
	QString ret = "";
	if (ui->chkCharaz->isChecked()) ret += "abcdefghijklmnopqrstuvwxyz";
	if (ui->chkCharAZ->isChecked()) ret += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (ui->chkCharNum->isChecked()) ret += "0123456789";
	if (ui->chkCharSpecial->isChecked()) ret += "@&%?,=:+*$#!'^~;/.";
	if (ui->chkCharUnderline->isChecked()) ret += "_";
	if (ui->chkCharMinus->isChecked()) ret += "-";
	if (ui->chkCharSpace->isChecked()) ret += " ";
	if (ui->chkCharBracket->isChecked()) ret += "()[]{}<>";
	addUniqueCharsToString(ret, ui->txtCharCustom->text());
	return ret;
}
void PasswordGeneratorWidget::addUniqueCharsToString(QString& str,
		const QString& chars_to_add) {
	for (int i = 0; i < chars_to_add.length(); ++i) {
		if (str.indexOf(chars_to_add[i]) == -1)
			str += chars_to_add[i];
	}
}

PasswordGeneratorWidget::Method PasswordGeneratorWidget::selectedMethod() {
	if (ui->cmbMethod->currentIndex() == 0) {
		return Method::Characters;
	} else {
		return Method::Template;
	}
}

void PasswordGeneratorWidget::getRandomBytes(unsigned char* buffer, int num) {
	int ret = secureRandomBytes(buffer, num);
	if (ret == 0) LOG(WARN, "Used less secure pseudo-random function for random numbers!");
	else if (ret == -1) {
		QMessageBox::critical(this, tr("Error"), tr("Failed to get random data"));
		throw 0;
	}
}

void PasswordGeneratorWidget::generateClicked() {
	QString password;
	try {
		switch (selectedMethod()) {
		case Method::Characters:
		{
			QString chars = selectedCharacters();
			int length = ui->txtPasswordLength->value();
			unsigned char* rand_values = new unsigned char[length];
			getRandomBytes(rand_values, length);
			//Note: we assume chars.length() <= 256 (otherwise 1 byte is not enough)
			for (int i = 0; i < length; ++i) {
				password += chars[rand_values[i] % chars.length()];
			}
			memset(rand_values, 0, sizeof(unsigned char)*length);
			delete[] (rand_values);
		}
		break;
		case Method::Template:
		{
			vector<QString> templates;
			generateTemplates(templates, ui->txtPasswordLengthTemplate->value(),
				ui->chkTemplateSpecial->isChecked() ? 1 : 0,
				ui->chkTemplateNumber->isChecked() ? 1 : 0);
			if (templates.empty()) return;
			int length = templates[0].length() + 4;
			unsigned char* rand_values = new unsigned char[length];
			getRandomBytes(rand_values, length);
			int temp_idx = (int) rand_values[0]
					| (((int) rand_values[1]) << 8)
					| (((int) rand_values[2]) << 16)
					| (((int) rand_values[3]) << 24);
			QString chosen_template = templates[temp_idx % templates.size()];
			QString chars;
			for (int i = 0; i < chosen_template.length(); ++i) {
				chars = QString::fromStdString(MasterPassword::charactersFromClass(
						chosen_template[i].toLatin1()));
				password += chars[rand_values[i + 4] % chars.length()];
			}
			memset(rand_values, 0, sizeof(unsigned char)*length);
			delete[] (rand_values);
		}

			break;
		}
	} catch (int) {
		//already handled
	}
	ui->txtPassword->setText(password);
	ui->txtPassword->selectAll();
	ui->txtPassword->setFocus();
}

void PasswordGeneratorWidget::generateTemplates(
		std::vector<QString>& templates_out, int length, int special_signs,
		int numbers) {
	const char* initial = nullptr;
	int modulo = (length - special_signs - numbers) % 4;
	switch (modulo) {
	case 0: initial = "Cvcc"; modulo = 4; break;
	case 1: initial = "C"; break;
	case 2: initial = "Cv"; break;
	case 3: initial = "Cvc"; break;
	}
	generateTemplatesRec(templates_out, initial, length - modulo, special_signs,
			numbers);
	generateTemplatesRec(templates_out, initial, length - modulo, special_signs,
			numbers);
}

void PasswordGeneratorWidget::generateTemplatesRec(
		std::vector<QString>& templates_out, QString prefix, int length,
		int special_signs, int numbers) {

	if (length < 0) return;
	if (length == 0 && special_signs == 0 && numbers == 0) {
		templates_out.push_back(prefix);
		return;
	}
	//1 left
	if (length == 1 && special_signs == 1 && numbers == 0) {
		templates_out.push_back(prefix + "o");
		return;
	}
	if (length == 1 && special_signs == 0 && numbers == 1) {
		templates_out.push_back(prefix + "n");
		return;
	}
	//2 left
	if (length == 2 && special_signs == 1 && numbers == 1) {
		templates_out.push_back(prefix + "no");
		templates_out.push_back(prefix + "on");
		return;
	}

	if (special_signs > 0 && numbers > 0) {
		generateTemplatesRec(templates_out, prefix+"noCvcc", length-6, special_signs-1, numbers-1);
		generateTemplatesRec(templates_out, prefix+"noCvcv", length-6, special_signs-1, numbers-1);
		generateTemplatesRec(templates_out, prefix+"onCvcc", length-6, special_signs-1, numbers-1);
		generateTemplatesRec(templates_out, prefix+"onCvcv", length-6, special_signs-1, numbers-1);
	}
	//add templates where special symbol and number is not necessarily together
	if (special_signs > 0) {
		generateTemplatesRec(templates_out, prefix+"oCvcc", length-5, special_signs-1, numbers);
		generateTemplatesRec(templates_out, prefix+"oCvcv", length-5, special_signs-1, numbers);
	}
	if (numbers > 0) {
		generateTemplatesRec(templates_out, prefix+"nCvcc", length-5, special_signs, numbers-1);
		generateTemplatesRec(templates_out, prefix+"nCvcv", length-5, special_signs, numbers-1);
	}
	generateTemplatesRec(templates_out, prefix+"Cvcc", length-4, special_signs, numbers);
	generateTemplatesRec(templates_out, prefix+"Cvcv", length-4, special_signs, numbers);
}

void PasswordGeneratorWidget::getPassword(QString& password) {
	password = ui->txtPassword->text();
}

void PasswordGeneratorWidget::showMethodExplanation() {
	QWhatsThis::showText(QCursor::pos(),
		tr(
		"<p><b>Method</b><br>"
		"The character-based method is purely random for the chosen characters and thus harder to guess.<br>"
		"The template-based method on the other hand is a bit less random but its passwords are easier to remember."
		"</p>"
		"<p><b>Password Strength</b><br>"
		"Shows the entropy bits, where an increase of 1 means twice the amount of brute-force attempts is needed.<br>"
		"Note that the statement holds for general passwords. "
		"A master password in qMasterPassword is harder to crack (compared to for example web passwords), "
		"because the algorithm was specifically designed to be CPU intensive."
		"</p>"
			), this);
}
