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

#ifndef ACTIVITYWIDGET_H
#define ACTIVITYWIDGET_H

#include <QFrame>
#include <QWidget>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPushButton>

#include "qactivitystreams.h"
#include "objectwidget.h"
#include "actorwidget.h"
#include "richtextlabel.h"
#include "shortactivitywidget.h"

#define MAX_WORD_LENGTH       40

//------------------------------------------------------------------------------

class ActivityWidget : public AbstractActivityWidget {
  Q_OBJECT

public:
  ActivityWidget(QASActivity* a, QWidget* parent=0);

  QString getId() const { return m_activity->id(); }
  void updateText();
  void refreshTimeLabels();
                   
public slots:
  void favourite();
  void repeat();
  void reply();

  void onObjectChanged();
  void onHasMoreClicked();

signals:
  void newReply(QASObject*);
  void like(QASObject*);
  void share(QASObject*);
  void linkHovered(const QString&);

private:
  QString recipientsToString(QASObjectList* rec);
  QString processText(QString old_text, bool getImages=false);
  QASActor* effectiveAuthor();
  void updateInfoText();

  void addHasMoreButton(QASObjectList* ol, int li);
  void updateFavourButton(bool wait = false);
  void updateShareButton(bool wait = false);
  void addObjectList(QASObjectList* ol);

  RichTextLabel* m_infoLabel;
  ObjectWidget* m_objectWidget;
  ActorWidget* m_actorWidget;

  QToolButton* m_favourButton;
  QToolButton* m_shareButton;
  QToolButton* m_commentButton;

  QHBoxLayout* m_buttonLayout;
  QVBoxLayout* m_rightLayout;
  QHBoxLayout* m_acrossLayout;
  
  QPushButton* m_hasMoreButton;

  QList<QASObject*> m_repliesList;
  QSet<QString> m_repliesMap;
};

#endif 
