#ifndef PERIODICTABLE_H
#define PERIODICTABLE_H

#include <QWidget>

namespace Ui {
class PeriodicTable;
}

class PeriodicTable : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY_MOVE(PeriodicTable)

public:
	explicit PeriodicTable(QWidget *parent = nullptr);
	~PeriodicTable() override;

private:
	Ui::PeriodicTable *ui;

public:
	QStringList GetCheckedElements() const;

};

#endif // PERIODICTABLE_H
