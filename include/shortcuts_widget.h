#ifndef SHORTCUTS_WIDGET_H
#define SHORTCUTS_WIDGET_H

#include <QDialog>

#include "main_window.h"

namespace Ui {
class ShortcutsWidget;
}

class ShortcutsWidget : public QDialog {
    Q_OBJECT

   public:
    explicit ShortcutsWidget(MainWindow& main_window);
    ~ShortcutsWidget();

   private:
    MainWindow& m_main_window;

    Ui::ShortcutsWidget* ui;
};

#endif  // SHORTCUTS_WIDGET_H
