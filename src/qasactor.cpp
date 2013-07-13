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

#include "qasactor.h"

#include <QRegExp>

//------------------------------------------------------------------------------

QASActor::QASActor(QString id, QObject* parent) :
  QASObject(id, parent),
  m_followed(false),
  m_isYou(false)
{
#if DEBUG >= 1
  qDebug() << "new Actor" << m_id;
#endif
}

//------------------------------------------------------------------------------

void QASActor::update(QVariantMap json) {
#if DEBUG >= 1
  qDebug() << "updating Actor" << m_id;
#endif
  bool ch = false;

  m_author = NULL;

  updateVar(json, m_url, "url", ch); 
  updateVar(json, m_displayName, "displayName", ch);
  updateVar(json, m_objectType, "objectType", ch);

  // this seems to be unreliable
  // updateVar(json, m_followed, "pump_io", "followed", ch);

  updateVar(json, m_summary, "summary", ch);
  updateVar(json, m_location, "location", "displayName", ch);

  if (json.contains("image")) {
    QVariantMap im = json["image"].toMap();
    updateUrlOrProxy(im, m_imageUrl, ch);
  }

  if (ch)
    emit changed();
}

//------------------------------------------------------------------------------

QASActor* QASActor::getActor(QVariantMap json, QObject* parent) {
  QString id = json["id"].toString();
  Q_ASSERT_X(!id.isEmpty(), "getActor", serializeJsonC(json));

  QASActor* act = s_objects.contains(id) ?
    qobject_cast<QASActor*>(s_objects[id]) : new QASActor(id, parent);
  s_objects.insert(id, act);

  act->update(json);
  return act;
}

//------------------------------------------------------------------------------

void QASActor::setFollowed(bool b) { 
  m_followed = b; 
  emit changed();
}

//------------------------------------------------------------------------------

QString QASActor::webFinger() const {
  QString wf = id();
  if (wf.startsWith("acct:"))
    wf.remove(0, 5);
  return wf;
}

//------------------------------------------------------------------------------

QString QASActor::displayNameOrWebFinger() const {
  if (displayName().isEmpty())
    return webFinger();
  return displayName();
}

//------------------------------------------------------------------------------

QString QASActor::displayNameOrWebFingerShort() const {
  return displayNameOrWebFinger().remove(QRegExp("[\\@- ].*"));
}

