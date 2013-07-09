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

#ifndef _OBJECTWIDGET_H_
#define _OBJECTWIDGET_H_

#include <QFrame>
#include <QWidget>

#include "qactivitystreams.h"
#include "shortobjectwidget.h"
#include "fullobjectwidget.h"

class ObjectWidget : public QFrame {
  Q_OBJECT

public:
  ObjectWidget(QASObject* obj, QWidget* parent = 0,
                      bool shortWidget=false);

  QASObject* object() const { return m_object; }

signals:
  void linkHovered(const QString&);
  void like(QASObject*);
  void share(QASObject*);
  void newReply(QASObject*);
  void moreClicked();
                          
private slots:
  void showMore();
  void onChanged();

private:
  FullObjectWidget* m_objectWidget;
  ShortObjectWidget* m_shortObjectWidget;

  QVBoxLayout* m_layout;

  QASObject* m_object;
  bool m_short;
};
  

#endif /* _OBJECTWIDGET_H_ */
