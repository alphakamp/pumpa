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

#include "objectwidget.h"

//------------------------------------------------------------------------------

ObjectWidget::ObjectWidget(QASObject* obj, QWidget* parent) :
  QFrame(parent),
  m_infoLabel(NULL),
  m_likesLabel(NULL),
  m_titleLabel(NULL),
  m_object(obj)
{
  m_layout = new QVBoxLayout(this);

  if (!obj->displayName().isEmpty()) {
    m_titleLabel = new QLabel("<b>" + obj->displayName() + "</b>");
    m_layout->addWidget(m_titleLabel);
  }

  if (obj->type() == "image") {
    m_imageLabel = new QLabel(this);
    m_imageLabel->setMaximumSize(320, 320);
    m_imageLabel->setFocusPolicy(Qt::NoFocus);
    m_imageUrl = obj->imageUrl();
    updateImage();

    m_layout->addWidget(m_imageLabel);
  }

  m_textLabel = new RichTextLabel(this);
  connect(m_textLabel, SIGNAL(linkHovered(const QString&)),
          this,  SIGNAL(linkHovered(const QString&)));
  m_layout->addWidget(m_textLabel);

  if (obj->type() == "comment") {
    m_infoLabel = new RichTextLabel(this);
    connect(m_infoLabel, SIGNAL(linkHovered(const QString&)),
            this, SIGNAL(linkHovered(const QString&)));

    m_layout->addWidget(m_infoLabel);
  }

  updateLikes();
  
  setLayout(m_layout);

  connect(m_object, SIGNAL(changed()), this, SLOT(onChanged()));
}

//------------------------------------------------------------------------------

void ObjectWidget::onChanged() {
  updateLikes();
}

//------------------------------------------------------------------------------

void ObjectWidget::setText(QString text) {
  m_textLabel->setText(text);
}

//------------------------------------------------------------------------------

void ObjectWidget::setInfo(QString text) {
  if (m_infoLabel == NULL) {
    qDebug() << "[WARNING] Trying to set info text to a non-comment object.";
    return;
  }
  m_infoLabel->setText(text);
}

//------------------------------------------------------------------------------

void ObjectWidget::fileReady(const QString& fn) {
  updateImage(fn);
}

//------------------------------------------------------------------------------

// FIXME this is duplicated in ActorWidget -> should be made more
// general and reused.
void ObjectWidget::updateImage(const QString& fileName) {
  static QString defaultImage = ":/images/broken_image.png";
  QString fn = fileName;

  if (fn.isEmpty()) {
    FileDownloader* fd = FileDownloader::get(m_imageUrl);

    if (fd->ready()) {
      fn = fd->fileName();
      fd->deleteLater();
    } else {
      connect(fd, SIGNAL(fileReady(const QString&)),
              this, SLOT(fileReady(const QString&)));
      fd->download();
    }
  }

  if (fn.isEmpty())
    fn = defaultImage;
  if (fn != m_localFile) {
    m_localFile = fn;
    QPixmap pix(m_localFile);
    if (pix.isNull()) {
      m_localFile = defaultImage;
      pix.load(m_localFile);
    }
    m_imageLabel->setPixmap(pix);
  }
}    

//------------------------------------------------------------------------------

void ObjectWidget::updateLikes() {
  if (!m_object->numLikes()) {
    if (m_likesLabel != NULL) {
      m_layout->removeWidget(m_likesLabel);
      delete m_likesLabel;
      m_likesLabel = NULL;
    }
    return;
  }

  if (m_likesLabel == NULL) {
    m_likesLabel = new RichTextLabel(this);
    connect(m_likesLabel, SIGNAL(linkHovered(const QString&)),
            this,  SIGNAL(linkHovered(const QString&)));
    m_layout->addWidget(m_likesLabel);
  }

  QString text;
  QASActorList* likes = m_object->likes();
  for (size_t i=0; i<likes->size(); i++) {
    QASActor* a = likes->at(i);
    text += QString("<a href=\"%1\">%2</a>")
      .arg(a->url())
      .arg(a->displayName());
    if (i != likes->size()-1)
      text += ", ";
  }

  if (likes->size() == 1)
    text += " likes";
  else 
    text += " like";
  text += " this.";
  
  m_likesLabel->setText(text);
}
