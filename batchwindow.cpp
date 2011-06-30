#include "batchwindow.h"
#include "ui_batchwindow.h"

BatchWindow::BatchWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::BatchWindow){
    ui->setupUi(this);
}

BatchWindow::~BatchWindow(){
	delete ui;
}

void BatchWindow::on_actionNew_Detail_Keyfinder_triggered(){
	DetailWindow* newWin = new DetailWindow(0);
	newWin->show();
}

void BatchWindow::on_actionNew_Batch_Keyfinder_triggered(){
	BatchWindow* newWin = new BatchWindow(0);
	newWin->show();
}

void BatchWindow::on_actionClose_Window_triggered(){
	this->close();
}

void BatchWindow::dragEnterEvent(QDragEnterEvent *e){
	// accept only local files
	if(e->mimeData()->hasUrls()  && !e->mimeData()->urls().at(0).toLocalFile().isEmpty()){
		e->acceptProposedAction();
	}
}

void BatchWindow::dropEvent(QDropEvent *e){
	// TODO with collision tests this can be slow, so make it asynchronous
	for(int i=0; i<e->mimeData()->urls().size(); i++){
		// test for collision
		bool addNew = true;
		QString fileName = e->mimeData()->urls().at(i).toLocalFile();
		for(int j=0; j<ui->tableWidget->rowCount(); j++){
			if(ui->tableWidget->item(j,0)->text() == fileName){
				addNew = false;
				break;
			}
		}
		if(addNew){
			int newRow = ui->tableWidget->rowCount(); // start at 0
			ui->tableWidget->insertRow(newRow);
			ui->tableWidget->setItem(newRow,0,new QTableWidgetItem());
			ui->tableWidget->item(newRow,0)->setText(fileName);
			ui->tableWidget->resizeColumnsToContents();
			ui->tableWidget->resizeRowsToContents();
		}
		//fileName = (std::string)e->mimeData()->urls().at(0).toLocalFile().toAscii();
	}
}


void BatchWindow::on_runBatchButton_clicked(){
	if(ui->tableWidget->rowCount()==0)
		return;
	// go
}
