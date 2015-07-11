#include <QFileDialog>
#include <QFile>
#include <QGraphicsPixmapItem>
#include <QListWidgetItem>
#include <QUndoStack>
#include "newmapdialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include  "assetlist.h"
#include "gdata.h"
#include "map.h"

namespace talga
{
  namespace editor
  {

    MainWindow::MainWindow(QWidget *parent) :
      QMainWindow(parent),
      ui(new Ui::MainWindow),
      pmImageViewScene(new QGraphicsScene),
      mTextures(),
      mTileWidth(32),
      mTileHeight(32)
    {
      ui->setupUi(this);

      //for updating the display every time you undo or redo
      connect(ui->historyView->stack(), SIGNAL(indexChanged(int)), ui->openGLWidget, SLOT(sl_updateGL()));

      //for when maps are loaded
      connect(GData::getInstance(), SIGNAL(sig_mapChanged(EditorMap*)), ui->openGLWidget, SLOT(sl_updateChangedMap(EditorMap*)));
      connect(GData::getInstance(), SIGNAL(sig_mapChanged(EditorMap*)), ui->historyView, SLOT(sl_updateChangedMap(EditorMap*)));
      connect(GData::getInstance(), SIGNAL(sig_mapChanged(EditorMap*)), ui->layerStack, SLOT(sl_updateChangedMap(EditorMap*)));
      connect(GData::getInstance(), SIGNAL(sig_mapChanged(EditorMap*)), ui->assetList, SLOT(sl_updateChangedMap(EditorMap*)));
      connect(GData::getInstance(), SIGNAL(sig_mapChanged(EditorMap*)), ui->imageView, SLOT(sl_updateChangedMap(EditorMap*)));
    }

    MainWindow::~MainWindow()
    {
      pmImageViewScene->clear();
      delete pmImageViewScene;

      GData::getInstance()->destroy();
      qDeleteAll(mTextures.values());
      mTextures.clear();

      delete ui;
    }

  }
}

void talga::editor::MainWindow::on_actionLoad_Assets_triggered()
{
  ui->assetList->sl_chooseAssets();
}

void talga::editor::MainWindow::on_actionSave_triggered()
{
  if (GData::getInstance()->hasMap() && GData::getInstance()->getCurrentMap()->getPath() != "")
  {
    GData::getInstance()->sl_saveMap();
  }
  else if (GData::getInstance()->hasMap())
  {
    on_actionSave_as_triggered();
  }
}

void talga::editor::MainWindow::on_actionOpen_triggered()
{
  QString files = QFileDialog::getOpenFileName(
                          this,
                          "Select one or more files to open",
                          "/home",
                          "Talga map (*.tmap)");
  if (files != "")
  {
    GData::getInstance()->sl_loadMap(files.toStdString());
  }
}

void talga::editor::MainWindow::on_actionSave_as_triggered()
{
  if (!GData::getInstance()->getCurrentMap() || !GData::getInstance()->hasMap()) return;

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                             "untitled",
                             tr("Talga map (*.tmap)"));

  if (fileName != "")
  {
    GData::getInstance()->getInstance()->sl_saveAs(fileName.toStdString());
  }
}

void talga::editor::MainWindow::on_actionAssetManager_triggered(bool checked)
{
    if (checked)
    {
      ui->assetManagerDock->show();
      ui->actionAssetManager->setChecked(true);
    }
    else
    {
      ui->assetManagerDock->hide();
      ui->actionAssetManager->setChecked(false);
    }
}

void talga::editor::MainWindow::on_actionAssetViewer_triggered(bool checked)
{
  if (checked)
  {
    ui->viewerDock->show();
    ui->actionAssetViewer->setChecked(true);
  }
  else
  {
    ui->actionAssetViewer->setChecked(false);
    ui->viewerDock->hide();
  }
}

void talga::editor::MainWindow::on_actionHistory_triggered(bool checked)
{
  if (checked)
  {
    ui->actionHistory->setChecked(true);
    ui->historyDock->show();
  }
  else
  {
    ui->actionHistory->setChecked(false);
    ui->historyDock->hide();
  }
}

void talga::editor::MainWindow::on_actionLayers_triggered(bool checked)
{
  if (checked)
  {
    ui->actionLayers->setChecked(true);
    ui->layerStackDock->show();
  }
  else
  {
    ui->actionLayers->setChecked(false);
    ui->layerStackDock->hide();
  }
}

void talga::editor::MainWindow::on_actionNew_triggered()
{
  NewMapDialog dialog;
  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    GData::getInstance()->setCurrentMap(dialog.getData());
}
