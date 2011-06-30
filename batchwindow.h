#ifndef BATCHWINDOW_H
#define BATCHWINDOW_H

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "detailwindow.h"

namespace Ui {
	class BatchWindow;
}

class BatchWindow : public QMainWindow{
	Q_OBJECT
public:
	explicit BatchWindow(QWidget *parent = 0);
	~BatchWindow();
	void dragEnterEvent(QDragEnterEvent*);
	void dropEvent(QDropEvent*);
private slots:
	void on_actionNew_Detail_Keyfinder_triggered();
	void on_actionNew_Batch_Keyfinder_triggered();
	void on_actionClose_Window_triggered();
	void on_runBatchButton_clicked();

private:
    Ui::BatchWindow *ui;
};

#endif // BATCHWINDOW_H
