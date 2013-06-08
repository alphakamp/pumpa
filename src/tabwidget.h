/*
  Copyright 2013 Mats Sjöberg
  
  This file is part of the Pumpa programme.

  Pumpa is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Pumpa is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License
  along with Pumpa.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>
#include <QTabBar>
#include <QTabWidget>
#include <QSignalMapper>

class TabWidget : public QTabWidget {
  Q_OBJECT

public:
  TabWidget(QWidget* parent=0);

  int addTab(QWidget* page, const QString& label, bool highlight=true);

  int addTab(QWidget* page, const QIcon& icon, const QString& label, 
             bool highlight=true);

public slots:
  void highlightTab(int index=-1);

  void deHighlightTab(int index=-1);

protected:
  void addHighlightConnection(QWidget* page, int index);

private:
  QSignalMapper* sMap;
};

#endif