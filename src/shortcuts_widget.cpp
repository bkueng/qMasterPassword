#include "include/shortcuts_widget.h"
#include "ui_shortcuts_widget.h"

ShortcutsWidget::ShortcutsWidget(MainWindow& main_window) :
	QDialog(&main_window),
	m_main_window(main_window),
	ui(new Ui::ShortcutsWidget)
{
	ui->setupUi(this);

	QTableWidget* shortcut_table = ui->tblShortcuts;
	shortcut_table->setRowCount((int) MainWindow::ShortcutAction::Count);
	shortcut_table->setColumnCount(2);
	QStringList labels;
	labels.append(tr("Shortcut(s)"));
	labels.append(tr("Description"));
	shortcut_table->setHorizontalHeaderLabels(labels);

	for (int i = 0; i < (int) MainWindow::ShortcutAction::Count; ++i) {
		MainWindow::ShortcutAction action = (MainWindow::ShortcutAction) i;
		const vector<QKeySequence>& key_sequences = main_window.shortcuts(action);
		QString shortcuts = "";
		for (const auto& shortcut : key_sequences)
			shortcuts += shortcut.toString() + ", ";
		shortcuts = shortcuts.mid(0, shortcuts.length() - 2);
		QTableWidgetItem* new_item = new QTableWidgetItem(shortcuts);
		new_item->setFlags(new_item->flags() & ~Qt::ItemIsEditable);
		shortcut_table->setItem(i, 0, new_item);
		new_item = new QTableWidgetItem(MainWindow::description(action));
		new_item->setFlags(new_item->flags() & ~Qt::ItemIsEditable);
		shortcut_table->setItem(i, 1, new_item);
	}
	shortcut_table->resizeColumnsToContents();
}

ShortcutsWidget::~ShortcutsWidget()
{
	delete ui;
}
