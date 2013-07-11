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

#include "fullobjectwidget.h"
#include "actorwidget.h"
#include "pumpa_defines.h"
#include "util.h"

#include <QDesktopServices>

//------------------------------------------------------------------------------

QSet<QString> s_allowedTags;

//------------------------------------------------------------------------------

FullObjectWidget::FullObjectWidget(QASObject* obj, QWidget* parent,
                                   bool childWidget) :
  QFrame(parent),
  m_infoLabel(NULL),
  m_likesLabel(NULL),
  m_sharesLabel(NULL),
  m_titleLabel(NULL),
  m_hasMoreButton(NULL),
  m_favourButton(NULL),
  m_shareButton(NULL),
  m_commentButton(NULL),
  m_followButton(NULL),
  m_object(obj),
  m_author(NULL),
  m_childWidget(childWidget)
{
  const QString objType = m_object->type();

  if (objType == "comment") {
    setLineWidth(1);
    setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
  }

  QVBoxLayout* rightLayout = new QVBoxLayout;
  rightLayout->setContentsMargins(0, 0, 0, 0);

  m_contentLayout = new QVBoxLayout;
  m_contentLayout->setContentsMargins(0, 0, 0, 0);

  if (!m_object->displayName().isEmpty()) {
    m_titleLabel = new QLabel("<b>" + m_object->displayName() + "</b>");
    m_contentLayout->addWidget(m_titleLabel);
  }

  if (objType == "image") {
    m_imageLabel = new ImageLabel(this);
    if (!m_object->fullImageUrl().isEmpty()) {
      connect(m_imageLabel, SIGNAL(clicked()), this, SLOT(imageClicked()));
      m_imageLabel->setCursor(Qt::PointingHandCursor);
    }
    m_imageUrl = m_object->imageUrl();
    updateImage();

    m_contentLayout->addWidget(m_imageLabel);
  }

  m_textLabel = new RichTextLabel(this);
  connect(m_textLabel, SIGNAL(linkHovered(const QString&)),
          this,  SIGNAL(linkHovered(const QString&)));
  m_contentLayout->addWidget(m_textLabel, 0, Qt::AlignTop);

  m_infoLabel = new RichTextLabel(this);
  connect(m_infoLabel, SIGNAL(linkHovered(const QString&)),
          this, SIGNAL(linkHovered(const QString&)));
  m_contentLayout->addWidget(m_infoLabel, 0, Qt::AlignTop);

  m_buttonLayout = new QHBoxLayout;

  if (objType == "note" || objType == "comment" || objType == "image") {
    m_favourButton = new TextToolButton(this);
    connect(m_favourButton, SIGNAL(clicked()), this, SLOT(favourite()));
    m_buttonLayout->addWidget(m_favourButton, 0, Qt::AlignTop);

    m_shareButton = new TextToolButton(this);
    connect(m_shareButton, SIGNAL(clicked()), this, SLOT(repeat()));
    m_buttonLayout->addWidget(m_shareButton, 0, Qt::AlignTop);

    m_commentButton = new TextToolButton("comment", this);
    connect(m_commentButton, SIGNAL(clicked()), this, SLOT(reply()));
    m_buttonLayout->addWidget(m_commentButton, 0, Qt::AlignTop);
  }

  if (objType == "person") {
    m_followButton = new TextToolButton(this);
    connect(m_followButton, SIGNAL(clicked()), this, SLOT(follow()));
    m_buttonLayout->addWidget(m_followButton, 0, Qt::AlignTop);
    m_author = m_object->asActor();
  }

  m_buttonLayout->addStretch();

  m_commentsLayout = new QVBoxLayout;
  onChanged();

  rightLayout->addLayout(m_contentLayout);
  rightLayout->addLayout(m_buttonLayout);
  rightLayout->addLayout(m_commentsLayout);

  bool smallActor = m_childWidget;

  QASActor* actor = m_author;
  if (!actor)
    actor = m_object->author();
  ActorWidget* actorWidget = new ActorWidget(actor, this, smallActor);

  QHBoxLayout* acrossLayout = new QHBoxLayout;
  acrossLayout->setSpacing(10);
  acrossLayout->addWidget(actorWidget, 0, Qt::AlignTop);
  acrossLayout->addLayout(rightLayout);
  
  setLayout(acrossLayout);

  connect(m_object, SIGNAL(changed()), this, SLOT(onChanged()));
}

//------------------------------------------------------------------------------

bool FullObjectWidget::hasValidIrtObject() {
  QASObject* irtObj = m_object->inReplyTo();
  return irtObj && !irtObj->id().isEmpty();
}

//------------------------------------------------------------------------------

void FullObjectWidget::onChanged() {
  updateLikes();
  updateShares();

  updateFavourButton();
  updateShareButton();
  if (m_commentButton)
    m_commentButton->setVisible(m_object->type() != "comment" ||
                                hasValidIrtObject());
  updateFollowButton();

  QString text = m_object->content();
  if (m_author) {
    text = m_author->summary();
    if (text.isEmpty())
      text = "[No description]";
  }

  setText(processText(text, true));

  updateInfoText();
  if (m_object->numReplies() > 0) {
    QASObjectList* ol = m_object->replies();
    if (ol) 
      connect(ol, SIGNAL(changed()), this, SLOT(onChanged()),
              Qt::UniqueConnection);
    addObjectList(ol);
  }
}

//------------------------------------------------------------------------------

void FullObjectWidget::setText(QString text) {
  m_textLabel->setText(text);
}

//------------------------------------------------------------------------------

void FullObjectWidget::updateInfoText() {
  if (m_infoLabel == NULL)
    return;

  QString infoStr;
  if (m_author) {
    QString aid = m_author->id();
    if (aid.startsWith("acct:"))
      aid.remove(0, 5);
    infoStr = QString("<a href=\"%2\">%1</a>").arg(aid).arg(m_object->url());
    
    QString location = m_author->location();
    if (!location.isEmpty())
      infoStr += " at " + location;
  } else {
    infoStr = QString("<a href=\"%2\">%1</a>").
      arg(relativeFuzzyTime(m_object->published())).
      arg(m_object->url());

    QASActor* author = m_object->author();
    if (author)
      infoStr = QString("<a href=\"%2\">%1</a> at ").
        arg(author->displayName()).
        arg(author->url()) + infoStr;
  }
  m_infoLabel->setText(infoStr);
}

//------------------------------------------------------------------------------

void FullObjectWidget::updateFavourButton(bool wait) {
  if (!m_favourButton)
    return;

  QString text = m_object->liked() ? "unlike" : "like";
  if (wait)
    text = "...";
  m_favourButton->setText(text);
}

//------------------------------------------------------------------------------

void FullObjectWidget::updateShareButton(bool /*wait*/) {
  if (!m_shareButton)
    return;

  m_shareButton->setText("share");
}

//------------------------------------------------------------------------------

bool FullObjectWidget::isFollowable() const {
  return m_object->type() == "person" && m_object->id().startsWith("acct:") &&
    m_author && !m_author->isYou();
}

//------------------------------------------------------------------------------

void FullObjectWidget::updateFollowButton(bool /*wait*/) {
  if (!m_followButton)
    return;
  
  if (!isFollowable()) {
    m_followButton->setVisible(false);
    return;
  }

  m_followButton->setVisible(true);
  m_followButton->setText(m_author->followed() ? "Stop following" : "Follow");
}

//------------------------------------------------------------------------------

void FullObjectWidget::updateImage() {
  FileDownloader* fd = FileDownloader::get(m_imageUrl, true);
  connect(fd, SIGNAL(fileReady()), this, SLOT(updateImage()),
          Qt::UniqueConnection);
  m_imageLabel->setPixmap(fd->pixmap(":/images/broken_image.png"));
}    

//------------------------------------------------------------------------------

void FullObjectWidget::updateLikes() {
  size_t nl = m_object->numLikes();

  if (nl <= 0) {
    if (m_likesLabel != NULL) {
      m_contentLayout->removeWidget(m_likesLabel);
      delete m_likesLabel;
      m_likesLabel = NULL;
    }
    return;
  }

  QASActorList* likes = m_object->likes();

  QString text;
  if (m_likesLabel == NULL) {
    m_likesLabel = new RichTextLabel(this);
    connect(m_likesLabel, SIGNAL(linkHovered(const QString&)),
            this,  SIGNAL(linkHovered(const QString&)));
    m_contentLayout->addWidget(m_likesLabel);
  }

  text = likes->actorNames();
  text += (nl==1 && !likes->onlyYou()) ? " likes" : " like";
  text += " this.";
  
  m_likesLabel->setText(text);
}

//------------------------------------------------------------------------------

void FullObjectWidget::updateShares() {
  size_t ns = m_object->numShares();
  if (!ns) {
    if (m_sharesLabel != NULL) {
      m_contentLayout->removeWidget(m_sharesLabel);
      delete m_sharesLabel;
      m_sharesLabel = NULL;
    }
    return;
  }

  if (m_sharesLabel == NULL) {
    m_sharesLabel = new RichTextLabel(this);
    connect(m_sharesLabel, SIGNAL(linkHovered(const QString&)),
            this,  SIGNAL(linkHovered(const QString&)));
    m_contentLayout->addWidget(m_sharesLabel);
  }

  QString text;
  if (m_object->shares()->size()) {
    text = m_object->shares()->actorNames();
    int others = ns-m_object->shares()->size();
    if (others > 0)
      text += QString(" and %1 other %2").arg(others).
        arg(others > 1 ? "persons" : "person");
    text += " shared this.";
  } else {
    if (ns == 1)
      text = "1 person shared this.";
    else
      text = QString("%1 persons shared this.").arg(ns);
  }
  
  m_sharesLabel->setText(text);
}

//------------------------------------------------------------------------------

void FullObjectWidget::imageClicked() {
  QString url = m_object->fullImageUrl();
  if (!url.isEmpty())
    QDesktopServices::openUrl(url);
}  

//------------------------------------------------------------------------------

void FullObjectWidget::addObjectList(QASObjectList* ol) {
  int li = 0; // index where to insert next widget in the layout
  int li_before = li;
  if (m_hasMoreButton != NULL)
    li++;

  /*
    For now we sort by time, or more accurately by whatever number the
    QASObject::sortInt() returns. Higher number is newer, goes further
    down the list.
  
    Comments' lists returned by the pump API are with newest at the
    top, so we start from the end, and can assume that the next one is
    always newer.
  */

  int i = 0; // index into m_repliesList
  for (size_t j=0; j<ol->size(); j++) {
    QASObject* replyObj = ol->at(ol->size()-j-1);
    QString replyId = replyObj->id();
    qint64 sortInt = replyObj->sortInt();

    while (i < m_repliesList.size() &&
           m_repliesList[i]->id() != replyId &&
           m_repliesList[i]->sortInt() < sortInt)
      i++;

    if (m_repliesMap.contains(replyId))
      continue;

    if (i < m_repliesList.size() && m_repliesList[i]->id() == replyId)
      continue;

    FullObjectWidget* ow = new FullObjectWidget(replyObj, this, true);

    connect(ow, SIGNAL(linkHovered(const QString&)),
            this, SIGNAL(linkHovered(const QString&)));
    connect(ow, SIGNAL(newReply(QASObject*)),
            this,  SIGNAL(newReply(QASObject*)));
    connect(ow, SIGNAL(like(QASObject*)), 
            this, SIGNAL(like(QASObject*)));
    connect(ow, SIGNAL(share(QASObject*)), 
            this, SIGNAL(share(QASObject*)));
    connect(ow, SIGNAL(follow(QString)),
            this, SIGNAL(follow(QString)));

    m_commentsLayout->insertWidget(li + i, ow);
    m_repliesList.insert(i, replyObj);
    m_repliesMap.insert(replyId);
  }

  if (ol->hasMore() && (qulonglong)m_repliesList.size() < ol->totalItems()) {
    // qDebug() << "[DEBUG]:" << "addHasMoreButton:"
    //          << ol->hasMore() << (qulonglong)m_repliesList.size()
    //          << ol->totalItems();
    addHasMoreButton(ol, li_before);
  } else if (m_hasMoreButton != NULL) {
    m_commentsLayout->removeWidget(m_hasMoreButton);
    delete m_hasMoreButton;
    m_hasMoreButton = NULL;
  }
}

//------------------------------------------------------------------------------

void FullObjectWidget::addHasMoreButton(QASObjectList* ol, int li) {
  QString buttonText = QString("Show all %1 replies").
    arg(ol->totalItems());
  if (m_hasMoreButton == NULL) {
    m_hasMoreButton = new QPushButton(this);
    m_hasMoreButton->setFocusPolicy(Qt::NoFocus);
    m_commentsLayout->insertWidget(li, m_hasMoreButton);
    connect(m_hasMoreButton, SIGNAL(clicked()), 
            this, SLOT(onHasMoreClicked()));
  }

  m_hasMoreButton->setText(buttonText);
}

//------------------------------------------------------------------------------

void FullObjectWidget::onHasMoreClicked() {
  m_hasMoreButton->setText("...");
  m_object->replies()->refresh();
}

//------------------------------------------------------------------------------

void FullObjectWidget::favourite() {
  updateFavourButton(true);
  emit like(m_object);
}

//------------------------------------------------------------------------------

void FullObjectWidget::repeat() {
  updateShareButton(true);
  emit share(m_object);
}

//------------------------------------------------------------------------------

void FullObjectWidget::reply() {
  emit newReply(m_object);
}

//------------------------------------------------------------------------------

void FullObjectWidget::follow() {
  if (isFollowable())
    emit follow(m_object->id());
}

//------------------------------------------------------------------------------

QString FullObjectWidget::processText(QString old_text, bool getImages) {
  if (s_allowedTags.isEmpty()) {
    s_allowedTags 
      << "br" << "p" << "b" << "i" << "blockquote" << "div"
      << "code" << "h1" << "h2" << "h3" << "h4" << "h5"
      << "em" << "ol" << "li" << "ul" << "hr" << "strong";
    s_allowedTags << "pre";
    s_allowedTags << "a";
    s_allowedTags << "img";
  }
  
  QString text = old_text.trimmed();
  int pos;

  // Shorten links that are too long, this is OK, since you can still
  // click the link.
  QRegExp rxa("<a\\s[^>]*href=([^>\\s]+)[^>]*>([^<]*)</a>");
  pos = 0;
  while ((pos = rxa.indexIn(text, pos)) != -1) {
    int len = rxa.matchedLength();
    QString url = rxa.cap(1);
    QString linkText = rxa.cap(2);

    if ((linkText.startsWith("http://") || linkText.startsWith("https://")) &&
        linkText.length() > MAX_WORD_LENGTH) {
      linkText = linkText.left(MAX_WORD_LENGTH-3) + "...";
      QString newText = QString("<a href=%1>%2</a>").arg(url).arg(linkText);
      text.replace(pos, len, newText);
      pos += newText.length();
    } else
      pos += len;
  }

  // Detect single HTML tags for filtering.
  QRegExp rx("<(\\/?)([a-zA-Z0-9]+)([^>]*)>");
  pos = 0;
  while ((pos = rx.indexIn(text, pos)) != -1) {
    int len = rx.matchedLength();
    QString slash = rx.cap(1);
    QString tag = rx.cap(2);
    QString inside = rx.cap(3);

    if (tag == "img") { // Replace img's with placeholder
      QString imagePlaceholder = "[image]";

      if (getImages) {
        QRegExp rxi("\\s+src=\"?(" URL_REGEX ")\"?");
        int spos = rxi.indexIn(inside);
        if (spos != -1) {
          QString imgSrc = rxi.cap(1);
          // qDebug() << "[DEBUG] processText: img" << imgSrc;
          
          FileDownloader* fd = FileDownloader::get(imgSrc, true);
          connect(fd, SIGNAL(fileReady()), this, SLOT(onChanged()),
                  Qt::UniqueConnection);
          if (fd->ready())
            imagePlaceholder = 
              QString("<img src=\"%1\" />").arg(fd->fileName());
        }
      }
      text.replace(pos, len, imagePlaceholder);
      pos += imagePlaceholder.length();
      // qDebug() << "[DEBUG] processText: removing image";
    } else if (s_allowedTags.contains(tag)) {
      pos += len;
    } else { // drop all other HTML tags
      if (tag != "span") 
        qDebug() << "[DEBUG] processText: dropping unsupported tag" << tag;
      text.remove(pos, len);
    }
  }

  text.replace("< ", "&lt; ");

  // remove trailing <br>:s
  while (text.endsWith("<br>"))
    text.chop(4);

  return text;
}
