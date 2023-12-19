#ifndef _HEADER_PASSWORD_GENERATOR_WIDGET_H
#define _HEADER_PASSWORD_GENERATOR_WIDGET_H

#include <QDialog>
#include <QString>

namespace Ui {
class PasswordGeneratorWidget;
}

class PasswordGeneratorWidget : public QDialog {
    Q_OBJECT

   public:
    explicit PasswordGeneratorWidget(QWidget* parent = 0);
    ~PasswordGeneratorWidget();

    void getPassword(QString& password);

   protected:
    bool eventFilter(QObject* obj, QEvent* ev);

   private:
    enum class Method { Characters, Template };
    Method selectedMethod();
    QString selectedCharacters();
    void addUniqueCharsToString(QString& str, const QString& chars_to_add);
    void getRandomBytes(unsigned char* buffer, int num);

    Ui::PasswordGeneratorWidget* ui;
    double m_current_entrypy_bits = 0.;

    /** generate disjoint templates. all have the same length */
    void generateTemplates(std::vector<QString>& templates_out, int length, int special_signs,
                           int numbers);
    void generateTemplatesRec(std::vector<QString>& templates_out, QString prefix, int length,
                              int special_signs, int numbers);

   private slots:
    void uiChanged();
    void generateClicked();
    void showMethodExplanation();
};

#endif  // _HEADER_PASSWORD_GENERATOR_WIDGET_H
