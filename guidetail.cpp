/*************************************************************************

  Copyright 2011-2013 Ibrahim Sha'ath

  This file is part of KeyFinder.

  KeyFinder is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  KeyFinder is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#include "guidetail.h"
#include "ui_detailwindow.h"

const int ROW_BIGCHROMA = 0;
const int ROW_MINICHROMA = 1;
const int ROW_RATEOFCHANGE = 2;

DetailWindow::DetailWindow(QWidget *parent, QString path) : QMainWindow(parent), ui(new Ui::DetailWindow){
  ui->setupUi(this);
  //: The title of the Detail window
  this->setWindowTitle(GuiStrings::getInstance()->appName() + GuiStrings::getInstance()->delim() + tr("Detailed Analysis"));
  //: A tooltip on the Detail window
  ui->chromaColourCombo->setToolTip(wrapToolTip(tr("Choose the colour scheme for the chromagram. Some provide higher contrast than others.")));
  //: A tooltip on the Detail window
  ui->colourScaleLabel->setToolTip(wrapToolTip(tr("This is the colour scale of the current chromagram. You can change the colour scheme using the dropdown below.")));
  connect(&analysisWatcher, SIGNAL(finished()), this, SLOT(analysisFinished()));
  allowDrops = true;
  ui->progressBar->setVisible(false);
  ui->runButton->setEnabled(false);
  layoutScaling();
  drawColourScale();
  drawPianoKeys();
  blankVisualisations();
  blankKeyLabel();
  if(path != ""){
    filePath = path;
    runAnalysis();
  }
}

DetailWindow::~DetailWindow(){
  analysisWatcher.cancel();
  delete ui;
}

QString DetailWindow::wrapToolTip(const QString& str){
  // This makes a tooltip rich text, which enables wrapping
  return "<FONT COLOR=black>" + str + "</FONT>";
}

void DetailWindow::dragEnterEvent(QDragEnterEvent *e){
  // accept only single, local files
  if(
     allowDrops
     && e->mimeData()->hasUrls()
     && e->mimeData()->urls().size() == 1
     && !e->mimeData()->urls().at(0).toLocalFile().isEmpty()
     ){
    e->acceptProposedAction();
  }
}

void DetailWindow::dropEvent(QDropEvent *e){
  allowDrops = false;
  filePath = e->mimeData()->urls().at(0).toLocalFile();
  runAnalysis();
}

void DetailWindow::runAnalysis(){
  // get latest preferences and redraw variable UI elements if they've changed since the last run.
  unsigned int chkOctaves = prefs.getOctaves();
  unsigned int chkOffset = prefs.getOffsetToC();
  prefs = Preferences();
  if(chkOctaves != prefs.getOctaves() || chkOffset != prefs.getOffsetToC() || ui->chromagramLabel->toolTip().left(4) == "Drag"){
    layoutScaling();
    drawPianoKeys();
    //: A tooltip on the Detail window
    ui->chromagramLabel->setToolTip(wrapToolTip(tr("This chromagram spans %1. The vertical axis represents musical frequencies as indicated by the piano keyboard. The horizontal axis splits the track into analysis windows of a few seconds each. The brighter the colour, the higher the energy found at that frequency during that window.")
      //: Part of a tooltip on the Detail window
      .arg(tr("%n octave(s)", "", prefs.getOctaves()))));
  }
  //: Text in the Batch window status bar
  say(tr("Analysing... "));
  ui->progressBar->setVisible(true);
  ui->chromaColourCombo->setEnabled(false);
  ui->runButton->setEnabled(false);
  // and proceed
  QList<AsyncFileObject> objects;
  objects.push_back(AsyncFileObject(filePath, prefs, -1));
  analysisFuture = QtConcurrent::mapped(objects, keyDetectionProcess);
  analysisWatcher.setFuture(analysisFuture);
}

void DetailWindow::analysisFinished(){
  QString error = analysisWatcher.result().errorMessage;
  if(error != ""){
    say(error);
    cleanUpAfterRun();
    return;
  }
  // Title bar
  TagLibMetadata md(filePath);
  QString shortName = md.getTitle();
  if(shortName == "")
    shortName = filePath.mid(filePath.lastIndexOf("/") + 1);
  this->setWindowTitle(GuiStrings::getInstance()->appName() + GuiStrings::getInstance()->delim() + tr("Detailed Analysis") + GuiStrings::getInstance()->delim() + shortName);
  // full chromagram
  chromagramImage = imageFromChromagram(analysisWatcher.result().fullChromagram);
  ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
  ui->chromagramLabel->setMinimumHeight(analysisWatcher.result().fullChromagram.getBands()+2);
  ui->chromagramLabel->setMinimumWidth(analysisWatcher.result().fullChromagram.getHops()+2);
  // one octave chromagram
  miniChromagramImage = imageFromChromagram(analysisWatcher.result().oneOctaveChromagram);
  ui->miniChromagramLabel->setPixmap(QPixmap::fromImage(miniChromagramImage));
  //: A tooltip on the Detail window
  ui->miniChromagramLabel->setToolTip(wrapToolTip(tr("This is the same chromagram data, reduced to a single octave.")));
  // harmonic change signal
  int rateOfChangePrecision = 100;
  int size = (signed)analysisWatcher.result().core.harmonicChangeSignal.size();
  harmonicChangeImage = QImage(size*chromaScaleH,rateOfChangePrecision,QImage::Format_Indexed8);
  prefs.setImageColours(harmonicChangeImage, (chromagram_colour_t) ui->chromaColourCombo->currentIndex());
  for(int h=0; h<size; h++){
    int value = analysisWatcher.result().core.harmonicChangeSignal[h] * rateOfChangePrecision;
    for(int y=0; y<rateOfChangePrecision; y++)
      for(int x=0; x<chromaScaleH; x++)
        harmonicChangeImage.setPixel(h*chromaScaleH+x, y, (rateOfChangePrecision - y > value ? 0 : 50));
  }
  ui->harmonicChangeLabel->setPixmap(QPixmap::fromImage(harmonicChangeImage));
  // Tooltip
  if(prefs.getSegmentation() == KeyFinder::SEGMENTATION_NONE){
    //: A tooltip on the Detail window
    ui->harmonicChangeLabel->setToolTip(wrapToolTip(tr("You are not using segmentation, so there is no harmonic change data to display.")));
  }else if(prefs.getSegmentation() == KeyFinder::SEGMENTATION_ARBITRARY){
    //: A tooltip on the Detail window
    ui->harmonicChangeLabel->setToolTip(wrapToolTip(tr("You are using arbitrary segmentation, so there is no harmonic change data to display.")));
  }else{
    //: A tooltip on the Detail window
    ui->harmonicChangeLabel->setToolTip(wrapToolTip(tr("This is the level of harmonic change detected in the chromagram over time. Peaks in this signal are used to segment the chromagram.")));
  }
  // Key estimates
  deleteKeyLabels();
  int lastChange = -1; // enable correct stretch policy for first segment
  for(int h = 0; h < (signed)analysisWatcher.result().core.segments.size(); h++){
    QLabel* newLabel = new QLabel(prefs.getKeyCode(analysisWatcher.result().core.segments[h].key));
    newLabel->setAlignment(Qt::AlignCenter);
    QPalette pal = newLabel->palette();
    pal.setColor(backgroundRole(), prefs.getKeyColour(analysisWatcher.result().core.segments[h].key));
    newLabel->setPalette(pal);
    newLabel->setFrameStyle(1);
    newLabel->setAutoFillBackground(true);
    newLabel->setMinimumHeight(20);
    newLabel->setMaximumHeight(30);
    if(prefs.getSegmentation() == KeyFinder::SEGMENTATION_NONE){
      //: A tooltip on the Detail window
      newLabel->setToolTip(wrapToolTip(tr("This row shows the key estimate for the unsegmented chromagram.")));
    }else if(prefs.getSegmentation() == KeyFinder::SEGMENTATION_ARBITRARY){
      //: A tooltip on the Detail window
      newLabel->setToolTip(wrapToolTip(tr("This row shows the key estimates for the arbitrary segments.")));
    }else{
      //: A tooltip on the Detail window
      newLabel->setToolTip(wrapToolTip(tr("This row shows the key estimates for the segments between peak harmonic changes.")));
    }
    ui->horizontalLayout_keyLabels->addWidget(newLabel, h - lastChange);
    keyLabels.push_back(newLabel);
    lastChange = h;
  }
  //: Text in the Batch window status bar; includes a key code at %1
  say(tr("Key estimate: %1").arg(prefs.getKeyCode(analysisWatcher.result().core.globalKeyEstimate)));
  cleanUpAfterRun();
}

void DetailWindow::cleanUpAfterRun(){
  ui->progressBar->setVisible(false);
  ui->chromaColourCombo->setEnabled(true);
  ui->runButton->setEnabled(true);
  allowDrops = true;
}

QImage DetailWindow::imageFromChromagram(const KeyFinder::Chromagram& ch){
  // 64 colours (plus black at index 0)
  // don't draw individual pixels; draw blocks of chromaScaleV*chromaScaleH. Sharpens image.
  QImage img = QImage(ch.getHops()*chromaScaleH,ch.getBands()*chromaScaleV,QImage::Format_Indexed8);
  prefs.setImageColours(img, (chromagram_colour_t) ui->chromaColourCombo->currentIndex());
  // get max to normalise
  float max = 0;
  for(unsigned int h = 0; h < ch.getHops(); h++){
    for(unsigned int b = 0; b < ch.getBands(); b++){
      float mag = ch.getMagnitude(h,b);
      if(mag>max) max = mag;
    }
  }
  // set pixels
  for(unsigned int h = 0; h < ch.getHops(); h++){
    for(unsigned int b = 0; b < ch.getBands(); b++){
      int pixVal = ch.getMagnitude(h,b) / max * img.colorCount() - 1;
      if(pixVal<1)
        pixVal = 1;
      for(int x=0; x<chromaScaleH; x++)
        for(int y=0; y<chromaScaleV; y++)
          img.setPixel(h*chromaScaleH+x, (ch.getBands()-1-b)*chromaScaleV+y, pixVal);
    }
  }
  return img;
}

void DetailWindow::say(const QString& s){
  ui->statusLabel->setText(s);
}

void DetailWindow::on_runButton_clicked(){
  if(filePath != "") runAnalysis();
}

void DetailWindow::on_chromaColourCombo_currentIndexChanged(int index){
  prefs.setImageColours(chromagramImage, (chromagram_colour_t) index);
  prefs.setImageColours(miniChromagramImage, (chromagram_colour_t) index);
  prefs.setImageColours(harmonicChangeImage, (chromagram_colour_t) index);
  prefs.setImageColours(colourScaleImage, (chromagram_colour_t) index);
  ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
  ui->miniChromagramLabel->setPixmap(QPixmap::fromImage(miniChromagramImage));
  ui->harmonicChangeLabel->setPixmap(QPixmap::fromImage(harmonicChangeImage));
  ui->colourScaleLabel->setPixmap(QPixmap::fromImage(colourScaleImage));
}

void DetailWindow::layoutScaling(){
  ui->gridLayout_Visualisation->setRowStretch(ROW_BIGCHROMA, prefs.getOctaves() * 2);
  ui->gridLayout_Visualisation->setRowStretch(ROW_MINICHROMA, 2);
  ui->gridLayout_Visualisation->setRowStretch(ROW_RATEOFCHANGE, 1);
  chromaScaleV = 5;
  chromaScaleH = 8 / prefs.getHopsPerFrame();
}

void DetailWindow::drawColourScale(){
  colourScaleImage = QImage(1, 65, QImage::Format_Indexed8);
  prefs.setImageColours(colourScaleImage, (chromagram_colour_t) ui->chromaColourCombo->currentIndex());
  for(int i=0; i<=64; i++)
    colourScaleImage.setPixel(0,64-i,i);
  ui->colourScaleLabel->setPixmap(QPixmap::fromImage(colourScaleImage));
}

void DetailWindow::blankVisualisations(){
  chromagramImage     = QImage(1, 1, QImage::Format_Indexed8);
  miniChromagramImage = QImage(1, 1, QImage::Format_Indexed8);
  harmonicChangeImage = QImage(1, 1, QImage::Format_Indexed8);
  prefs.setImageColours(chromagramImage,     (chromagram_colour_t) ui->chromaColourCombo->currentIndex());
  prefs.setImageColours(miniChromagramImage, (chromagram_colour_t) ui->chromaColourCombo->currentIndex());
  prefs.setImageColours(harmonicChangeImage, (chromagram_colour_t) ui->chromaColourCombo->currentIndex());
  chromagramImage.setPixel(0,0,0);
  miniChromagramImage.setPixel(0,0,0);
  harmonicChangeImage.setPixel(0,0,0);
  ui->chromagramLabel->setPixmap(QPixmap::fromImage(chromagramImage));
  ui->miniChromagramLabel->setPixmap(QPixmap::fromImage(miniChromagramImage));
  ui->harmonicChangeLabel->setPixmap(QPixmap::fromImage(harmonicChangeImage));
  //: The initial help tooltip on the Detail window
  QString blank = wrapToolTip(tr("Drag an audio file onto the window."));
  ui->chromagramLabel->setToolTip(blank);
  ui->miniChromagramLabel->setToolTip(blank);
  ui->harmonicChangeLabel->setToolTip(blank);
}

void DetailWindow::deleteKeyLabels(){
  for(int i = keyLabels.size()-1; i >= 0; i--){
    delete keyLabels[i];
    keyLabels.pop_back();
  }
}

void DetailWindow::blankKeyLabel(){
  deleteKeyLabels();
  QLabel* dummyLabel = new QLabel();
  QPalette pal = dummyLabel->palette();
  pal.setColor(backgroundRole(), Qt::black);
  dummyLabel->setPalette(pal);
  dummyLabel->setAutoFillBackground(true);
  dummyLabel->setMinimumHeight(20);
  dummyLabel->setMaximumHeight(30);
  dummyLabel->setToolTip(wrapToolTip(tr("Drag an audio file onto the window.")));
  ui->horizontalLayout_keyLabels->addWidget(dummyLabel);
  keyLabels.push_back(dummyLabel);
}

void DetailWindow::drawPianoKeys(){
  int scale = 10;
  QImage pianoImage = QImage(1,prefs.getOctaves()*12*scale,QImage::Format_Indexed8);
  QImage miniPianoImage = QImage(1,12*scale,QImage::Format_Indexed8);
  pianoImage.setColor(0,qRgb(255,255,255));	// white key
  pianoImage.setColor(1,qRgb(0,0,0));				// black key
  pianoImage.setColor(2,qRgb(127,127,127));	// boundary colour
  miniPianoImage.setColor(0,qRgb(255,255,255));
  miniPianoImage.setColor(1,qRgb(0,0,0));
  miniPianoImage.setColor(2,qRgb(127,127,127));
  // reverse of octave for visual representation (ending at A by default)
  QString octaveRev = "bwbwwbwbwwbw";
  if(prefs.getOffsetToC())
    octaveRev = octaveRev.right(3) + octaveRev.left(9);
  for(unsigned int o = 0; o < prefs.getOctaves(); o++){
    for(int s = 0; s < 12; s++){
      for(int px = 0; px < scale-1; px++){
        pianoImage.setPixel(0,(o*12*scale)+(s*scale)+px,(octaveRev[s] == 'b' ? 1 : 0));
        miniPianoImage.setPixel(0,(s*scale)+px,(octaveRev[s] == 'b' ? 1 : 0));
      }
      pianoImage.setPixel(0,(o*12*scale)+(s*scale)+scale-1,2);
      miniPianoImage.setPixel(0,(s*scale)+scale-1,2);
    }
  }
  ui->pianoKeysLabel->setPixmap(QPixmap::fromImage(pianoImage));
  ui->miniPianoKeysLabel->setPixmap(QPixmap::fromImage(miniPianoImage));
}
