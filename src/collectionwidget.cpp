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
  ASWidget(parent)
{}

//------------------------------------------------------------------------------

QASAbstractObjectList* CollectionWidget::initList(QString endpoint,
                                                  QObject* parent) {
  m_asMode = QAS_COLLECTION;
  return QASCollection::initCollection(endpoint, parent);
}

//------------------------------------------------------------------------------

ObjectWidgetWithSignals*
CollectionWidget::createWidget(QASAbstractObject* aObj, bool& countAsNew) {
  QASActivity* act = qobject_cast<QASActivity*>(aObj);
  if (!act) {
    qDebug() << "ERROR CollectionWidget::createWidget passed non-activity";
    return NULL;
  }

  ActivityWidget* aw = new ActivityWidget(act, this);
  connect(aw, SIGNAL(showContext(QASObject*)),
          this, SIGNAL(showContext(QASObject*)));

  QASObject* obj = act->object();
  if (obj)
    connect(obj, SIGNAL(request(QString, int)), 
            this, SIGNAL(request(QString, int)), Qt::UniqueConnection);

  countAsNew = !act->actor()->isYou();
  return aw;
}

//------------------------------------------------------------------------------

void CollectionWidget::fetchNewer() {
  emit request(m_list->url(), m_asMode | QAS_NEWER);
}
