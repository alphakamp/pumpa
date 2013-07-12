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

#include "collectionwidget.h"
#include "pumpa_defines.h"
#include "activitywidget.h"

#include <QDebug>

//------------------------------------------------------------------------------

CollectionWidget::CollectionWidget(QWidget* parent) :
  ASWidget(parent),
  m_collection(NULL)
{}

//------------------------------------------------------------------------------

void CollectionWidget::setEndpoint(QString endpoint) {
  clear();

  m_collection = QASCollection::initCollection(endpoint,
                                               parent()->parent()->parent());
  connect(m_collection, SIGNAL(changed(bool)), this, SLOT(update(bool)),
          Qt::UniqueConnection);
  connect(m_collection, SIGNAL(request(QString, int)),
          this, SIGNAL(request(QString, int)), Qt::UniqueConnection);
}

//------------------------------------------------------------------------------

void CollectionWidget::fetchNewer() {
  emit request(m_collection->prevLink(), QAS_COLLECTION | QAS_NEWER);
}

//------------------------------------------------------------------------------

void CollectionWidget::fetchOlder() {
  QString nextLink = m_collection->nextLink();
  if (!nextLink.isEmpty())
    emit request(nextLink, QAS_COLLECTION | QAS_OLDER);
}

//------------------------------------------------------------------------------

void CollectionWidget::update(bool older) {
  /* 
     We assume m_collection contains all objects, but new ones might
     have been added. Go through from top (newest) to bottom. Add any
     non-existing to top (going down from there).
  */

  int li = older ? m_itemLayout->count() : 0;
  int newCount = 0;

  for (size_t i=0; i<m_collection->size(); i++) {
    QASActivity* activity = m_collection->at(i);

    QASObject* obj = activity->object();
    if (obj->isDeleted())
      continue;

    if (m_activity_set.contains(activity))
      continue;
    m_activity_set.insert(activity);

    QString verb = activity->verb();
    
    ActivityWidget* aw = new ActivityWidget(activity, this);
    connect(aw, SIGNAL(linkHovered(const QString&)),
            this,  SIGNAL(linkHovered(const QString&)));
    connect(aw, SIGNAL(newReply(QASObject*)),
            this,  SIGNAL(newReply(QASObject*)));
    connect(aw, SIGNAL(like(QASObject*)),
            this,  SIGNAL(like(QASObject*)));
    connect(aw, SIGNAL(share(QASObject*)),
            this,  SIGNAL(share(QASObject*)));
    connect(aw, SIGNAL(follow(QString, bool)),
            this, SIGNAL(follow(QString, bool)));

    connect(aw, SIGNAL(showContext(QASObject*)),
            this, SIGNAL(showContext(QASObject*)));

    if (obj)
      connect(obj, SIGNAL(request(QString, int)), 
              this, SIGNAL(request(QString, int)), Qt::UniqueConnection);
    
    m_itemLayout->insertWidget(li++, aw);

    m_shown_objects.insert(obj);

    if (!activity->actor()->isYou())
      newCount++;
  }

  if (newCount && !isVisible() && !m_firstTime)
    emit highlightMe();
  m_firstTime = false;
}

