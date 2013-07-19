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

#include "activitywidget.h"
#include "texttoolbutton.h"
#include "pumpa_defines.h"

#include <QDebug>

//------------------------------------------------------------------------------

ActivityWidget::ActivityWidget(QASActivity* a, QWidget* parent) :
  ObjectWidgetWithSignals(parent),
  m_objectWidget(NULL),
  m_activity(a)
{
  const QString verb = m_activity->verb();
  QASObject* obj = m_activity->object();
  QString objType = obj->type();

  bool fullObject = (verb == "post");

  m_textLabel = new RichTextLabel(this);
  connect(m_textLabel, SIGNAL(linkHovered(const QString&)),
          this,  SIGNAL(linkHovered(const QString&)));

  if (!obj->content().isEmpty() || !obj->displayName().isEmpty()
      || (objType == "image" && !obj->imageUrl().isEmpty()))
    m_objectWidget = makeObjectWidgetAndConnect(obj, !fullObject);

  QVBoxLayout* layout = new QVBoxLayout;
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_textLabel, 0, Qt::AlignTop);

  if (m_objectWidget)
    layout->addWidget(m_objectWidget, 0, Qt::AlignTop);
  layout->addWidget(new QLabel("<hr />"));

  updateText();

  QASActor* actor = m_activity->actor();
  if (QASActivity::isLikeVerb(verb) && actor && !actor->isYou())
    obj->refresh();

  setLayout(layout);
}

//------------------------------------------------------------------------------

void ActivityWidget::onObjectChanged() {
  updateText();
}

//------------------------------------------------------------------------------

void ActivityWidget::refreshTimeLabels() {
  if (m_objectWidget)
    m_objectWidget->refreshTimeLabels();
}

//------------------------------------------------------------------------------

void ActivityWidget::updateText() {
  QString verb = m_activity->verb();
  QString text = m_activity->content();
  QString objType = m_activity->object()->type();

  QString generatorName = m_activity->generatorName();
  if (!generatorName.isEmpty() && (verb != "share"))
    text += QString(tr(" via %1")).arg(generatorName);

  if (verb == "post" && (objType == "note" || objType == "image")) {
    if (m_activity->hasTo())
      text += " " + tr("To:") +" " + recipientsToString(m_activity->to());
    
    if (m_activity->hasCc())
      text += " " + tr("CC:") + " " + recipientsToString(m_activity->cc());
  }

  m_textLabel->setText(text);
}
 
//------------------------------------------------------------------------------

QString ActivityWidget::recipientsToString(QASObjectList* rec) {
  if (!rec)
    return "";

  QStringList ret;

  for (size_t i=0; i<rec->size(); ++i) {
    QASObject* r = rec->at(i);
    if (r->type() == "collection" && r->id() == PUBLIC_RECIPIENT_ID) {
      ret << tr("Public");
    } else {
      QString name = r->displayName();
      QString url = r->url();

      if (url.isEmpty())
        ret << name;
      else
        ret << QString("<a href=\"%1\">%2</a>").arg(url).arg(name);
    }
  }

  return ret.join(", ");
}

//------------------------------------------------------------------------------

ObjectWidget* ActivityWidget::makeObjectWidgetAndConnect(QASObject* obj,
                                                         bool shortObject) {
  ObjectWidget* ow = new ObjectWidget(obj, this, shortObject);

  ObjectWidgetWithSignals::connectSignals(ow, this);
  connect(ow, SIGNAL(showContext(QASObject*)),
          this, SIGNAL(showContext(QASObject*)));

  connect(obj, SIGNAL(changed()), this, SLOT(onObjectChanged()),
          Qt::UniqueConnection);

  return ow;
}
