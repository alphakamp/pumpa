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

#include "shortobjectwidget.h"
#include "util.h"

#include <QVBoxLayout>
#include "texttoolbutton.h"

//------------------------------------------------------------------------------

ShortObjectWidget::ShortObjectWidget(QASObject* obj, QWidget* parent) :
  QFrame(parent),
  m_object(obj),
  m_actor(NULL)
{
  static QSet<QString> expandableTypes;
  if (expandableTypes.isEmpty())
    expandableTypes << "person" << "note" << "comment";

  connect(m_object, SIGNAL(changed()), this, SLOT(onChanged()));

  m_textLabel = new RichTextLabel(this, true);

  m_actorWidget = new ActorWidget(NULL, this, true);
  updateAvatar();

  TextToolButton* moreButton = NULL;
  
  if (expandableTypes.contains(m_object->type())) {
    moreButton = new TextToolButton("+", this);
    connect(moreButton, SIGNAL(clicked()), this, SIGNAL(moreClicked()));
  }
  
  QHBoxLayout* acrossLayout = new QHBoxLayout;
  // acrossLayout->setSpacing(10);
  acrossLayout->setContentsMargins(0, 0, 0, 0);
  acrossLayout->addWidget(m_actorWidget, 0, Qt::AlignVCenter);
  acrossLayout->addWidget(m_textLabel, 0, Qt::AlignVCenter);
  if (moreButton)
    acrossLayout->addWidget(moreButton, 0, Qt::AlignVCenter);

  updateText();
  
  setLayout(acrossLayout);

}

//------------------------------------------------------------------------------

void ShortObjectWidget::updateAvatar() {
  QASActor* m_actor = qobject_cast<QASActor*>(m_object);
  if (!m_actor)
    m_actor = m_object->author();
  m_actorWidget->setActor(m_actor);
}

//------------------------------------------------------------------------------

void ShortObjectWidget::updateText() {
  QString content = objectExcerpt(m_object);

  QString text;
  QASActor* author = m_object->author();
  if (author && !author->displayName().isEmpty())
    text = author->displayName();
  if (!content.isEmpty())
    text += (text.isEmpty() ? "" : ": ") + content;

  m_textLabel->setText(text);
}

//------------------------------------------------------------------------------

void ShortObjectWidget::onChanged() {
  updateAvatar();
  updateText();
}

//------------------------------------------------------------------------------

QString ShortObjectWidget::objectExcerpt(QASObject* obj) {
  QString text = obj->displayName();
  if (text.isEmpty()) {
    text = obj->content();
  }
  if (!text.isEmpty()) {
    text.replace(QRegExp(HTML_TAG_REGEX), " ");
  } else {
    QString t = obj->type();
    text = (t == "image" ? "an " : "a ") + t;
  }
  return text;
}
