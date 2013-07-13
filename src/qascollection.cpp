/*
  Copyright 2013 Mats Sjöberg
  
  This file is part of the Pumpa programme.

  Pumpa is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Pumpa is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Pumpa.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qascollection.h"

#include "util.h"

//------------------------------------------------------------------------------

QMap<QString, QASCollection*> QASCollection::s_collections;

void QASCollection::clearCache() { deleteMap<QASCollection*>(s_collections); }

//------------------------------------------------------------------------------

QASCollection::QASCollection(QString url, QObject* parent) :
  QASAbstractObject(QAS_COLLECTION, parent),
  m_url(url),
  m_totalItems(0)
{
#if DEBUG >= 1
  qDebug() << "new Collection" << m_url;
#endif
}

//------------------------------------------------------------------------------

void QASCollection::update(QVariantMap json, bool older) {
  bool ch = false;

  updateVar(json, m_displayName, "displayName", ch);
  updateVar(json, m_totalItems, "totalItems", ch);

  updateVar(json, m_prevLink, "links", "prev", "href", ch);
  updateVar(json, m_nextLink, "links", "next", "href", ch);

  // We assume that collections come in as newest first, so we add
  // items starting from the top going downwards. Or if older=true
  // starting from the end and going downwards (appending).

  // Start adding from the top or bottom, depending on value of older.
  int mi = older ? m_items.size() : 0;


  QVariantList items_json = json["items"].toList();
  for (int i=0; i<items_json.count(); i++) {
    QASActivity* act = QASActivity::getActivity(items_json.at(i).toMap(),
                                                parent());
    if (m_item_set.contains(act))
      continue;

    m_items.insert(mi++, act);
    m_item_set.insert(act);
    connectSignals(act);

    ch = true;
  }

  if (ch)
    emit changed(older);
}

//------------------------------------------------------------------------------

QASCollection* QASCollection::getCollection(QVariantMap json, QObject* parent,
                                            int id) {
  QString url = json["url"].toString();
  if (url.isEmpty())
    return NULL;

  QASCollection* coll = s_collections.contains(url) ? s_collections[url] :
    new QASCollection(url, parent);
  s_collections.insert(url, coll);

  coll->update(json, id & QAS_OLDER);
  return coll;
}

//------------------------------------------------------------------------------

QASCollection* QASCollection::initCollection(QString url, QObject* parent) {
  if (s_collections.contains(url))
    return s_collections[url];
  
  QASCollection* coll = new QASCollection(url, parent);
  s_collections.insert(url, coll);

  return coll;
}
