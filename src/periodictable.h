#ifndef PERIODICTABLE_H
#define PERIODICTABLE_H

#include <QWidget>

namespace Ui {
class PeriodicTable;
}

class PeriodicTable : public QWidget
{
	Q_OBJECT

public:
	explicit PeriodicTable(QWidget *parent = nullptr);
	~PeriodicTable();

private:
	Ui::PeriodicTable *ui;
};

#endif // PERIODICTABLE_H
