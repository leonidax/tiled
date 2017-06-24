/*
 * %finddialog.cpp%
 * Copyright 2017, Your Name <leon.moctezuma@gmail.com>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gotodialog.h"

#include "documentmanager.h"
#include "mapview.h"
#include "mapscene.h"
#include "mapreader.h"
#include "mapdocument.h"
#include "maprenderer.h"

using namespace Tiled::Internal;
using namespace Tiled;

GotoDialog *GotoDialog::mInstance;

GotoDialog::GotoDialog(QWidget *parent, Qt::WindowFlags f )
    : QDialog(parent, f), mHighlightTile(), mMessageBox(this)
{
    setWindowTitle(tr("Go to"));

    QGroupBox *horizontalGroupBox = new QGroupBox(tr("Go to tile"));
    QHBoxLayout *layout = new QHBoxLayout;
    horizontalGroupBox->setLayout(layout);
    QLabel *labelX = new QLabel(tr("X:"));
    QLabel *labelY = new QLabel(tr("Y:"));
    lineEditX = new QLineEdit(tr("0"));
    lineEditY = new QLineEdit(tr("0"));
    QPushButton* goButton = new QPushButton(tr("Go"));

    lineEditX->setMaximumWidth(50);
    lineEditY->setMaximumWidth(50);
    lineEditX->setValidator(new QIntValidator);
    lineEditY->setValidator(new QIntValidator);

    layout->addWidget(labelX);
    layout->addWidget(lineEditX);
    layout->addWidget(labelY);
    layout->addWidget(lineEditY);
    layout->addWidget(goButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(horizontalGroupBox);

    connect(goButton, &QAbstractButton::clicked, this, &GotoDialog::goToCoordinates);

    setLayout(mainLayout);

    mMessageBox.setStandardButtons(QMessageBox::Ok);
    mMessageBox.setIcon(QMessageBox::Critical);
}

GotoDialog* GotoDialog::showDialog()
{
    if (!mInstance) {
        QWidget *parentWidget = QApplication::activeWindow();
        mInstance = new GotoDialog(parentWidget, Qt::Tool);
    }

    mInstance->show();
    mInstance->raise();
    mInstance->activateWindow();

    return mInstance;
}

void GotoDialog::goToCoordinates()
{
    QString textX = lineEditX->text();
    QString textY = lineEditY->text();

    MapView *mapView = DocumentManager::instance()->currentMapView();

    if (mapView) {
        MapDocument *mapDocument = mapView->mapScene()->mapDocument();
        Map *map = mapDocument->map();
        int x = textX.toInt();
        int y = textY.toInt();
        int width = map->width();
        int height = map->height();

        if ( (x >= 0 && x < width) && (y >= 0 && y < height)) {
            MapRenderer *renderer = mapDocument->renderer();
            QPointF point = renderer->tileToScreenCoords(textX.toDouble(),textY.toDouble());

            mapView->centerOn(point);

            QRect region(textX.toInt(),textY.toInt(),1,1);

            mHighlightTile.setZValue(10000);
            mHighlightTile.setMapDocument(mapDocument);
            mapView->mapScene()->addItem(&mHighlightTile);
            mHighlightTile.setVisible(true);
            mHighlightTile.setTileRegion(region);
            mHighlightTile.animate();

            close();
        } else {
            mMessageBox.setText(tr("Coordinate ( %1 , %2 ) out map range with size [ %3 , %4 ]")
                                .arg(textX,textY,QString::number(width),QString::number(height)));
            mMessageBox.exec();
        }
    }
}