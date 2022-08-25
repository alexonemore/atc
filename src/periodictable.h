#ifndef PERIODICTABLE_H
#define PERIODICTABLE_H

#include <QWidget>
#include <QPushButton>
#include <QVector>

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
	QVector<QPushButton*> buttons;

public:
	QStringList GetCheckedElements() const;
	void EnableButtons(const QStringList& elements);

};

#endif // PERIODICTABLE_H
