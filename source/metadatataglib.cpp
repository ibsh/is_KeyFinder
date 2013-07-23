/*************************************************************************

  Copyright 2011-2013 Ibrahim Sha'ath

  This file is part of KeyFinder.

  KeyFinder is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  KeyFinder is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#include "metadatataglib.h"

/*
  Note that a lot remains untested here. For example, I didn't have any ASF or
  APE files to hand while writing it, and I wouldn't be surprised if there are
  many edge cases, though they should at least fail gracefully.
*/

QMutex constructor_mutex; // global mutex on file resolution

TagLibMetadata::TagLibMetadata(const QString& filePath) : f(NULL) {

  QMutexLocker locker(&constructor_mutex); // mutex the constructor

  QString fileExt = filePath.mid(filePath.lastIndexOf(".") + 1).toLower();

#ifdef Q_OS_WIN
  // Using utf16_to_utf8 here, as per decoderlibav, leads to a null file reference.
  const wchar_t* filePathCh = reinterpret_cast<const wchar_t*>(filePath.constData());
#else
  QByteArray encodedPath = QFile::encodeName(filePath);
  const char* filePathCh = encodedPath;
#endif

  if (fileExt == "mp3")
    f = new TagLib::MPEG::File(filePathCh);
  if (fileExt == "aif" || fileExt == "aiff")
    f = new TagLib::RIFF::AIFF::File(filePathCh);
  if (fileExt == "wav")
    f = new TagLib::RIFF::WAV::File(filePathCh);
  if (fileExt == "ogg")
    f = new TagLib::Ogg::Vorbis::File(filePathCh);
  if (fileExt == "oga") {
    f = new TagLib::Ogg::FLAC::File(filePathCh);
    if (f == NULL || !f->isValid()) {
      if (!f->isValid()) delete f;
      f = new TagLib::Ogg::Vorbis::File(filePathCh);
    }
  }
  if (fileExt == "flac")
    f = new TagLib::FLAC::File(filePathCh);
  if (fileExt == "ape")
    f = new TagLib::APE::File(filePathCh);
  if (fileExt == "mpc")
    f = new TagLib::MPC::File(filePathCh);
  if (fileExt == "wv")
    f = new TagLib::WavPack::File(filePathCh);
  if (fileExt == "spx")
    f = new TagLib::Ogg::Speex::File(filePathCh);
  if (fileExt == "tta")
    f = new TagLib::TrueAudio::File(filePathCh);
#ifdef TAGLIB_WITH_MP4
  if (fileExt == "m4a" || fileExt == "m4b" || fileExt == "m4p" || fileExt == "mp4" || fileExt == "3g2")
    f = new TagLib::MP4::File(filePathCh);
#endif
#ifdef TAGLIB_WITH_ASF
  if (fileExt == "wma" || fileExt == "asf")
    f = new TagLib::ASF::File(filePathCh);
#endif

  locker.unlock();

  if (f != NULL) {
    if (f->isValid()) {
      return; // everything's fine.
    } else {
      delete f;
      f = NULL;
    }
  }

#ifdef Q_OS_WIN
  qWarning("TagLib returned NULL File for %s", utf16_to_utf8(filePathCh));
#else
  qWarning("TagLib returned NULL File for %s", filePathCh);
#endif
  return;
}

TagLibMetadata::~TagLibMetadata() {
  if (f != NULL) delete f;
}

QString TagLibMetadata::getByTagEnum(metadata_tag_t tag) const {
  if      (tag == METADATA_TAG_TITLE)    return getTitle();
  else if (tag == METADATA_TAG_ARTIST)   return getArtist();
  else if (tag == METADATA_TAG_ALBUM)    return getAlbum();
  else if (tag == METADATA_TAG_COMMENT)  return getComment();
  else if (tag == METADATA_TAG_GROUPING) return getGrouping();
  else if (tag == METADATA_TAG_KEY)      return getKey();
  return QString("");
}

QString TagLibMetadata::getTitle() const {
  if (f == NULL || !f->isValid())
    return GuiStrings::getInstance()->notApplicable();
  TagLib::String out = f->tag()->title();
  return QString::fromUtf8(out.toCString(true));
}

QString TagLibMetadata::getArtist() const {
  if (f == NULL || !f->isValid())
    return GuiStrings::getInstance()->notApplicable();
  TagLib::String out = f->tag()->artist();
  return QString::fromUtf8(out.toCString(true));
}

QString TagLibMetadata::getAlbum() const {
  if (f == NULL || !f->isValid())
    return GuiStrings::getInstance()->notApplicable();
  TagLib::String out = f->tag()->album();
  return QString::fromUtf8(out.toCString(true));
}

QString TagLibMetadata::getComment() const {
  if (f == NULL || !f->isValid())
    return GuiStrings::getInstance()->notApplicable();
  TagLib::FLAC::File* fileTestFlac = dynamic_cast<TagLib::FLAC::File*>(f);
  if (fileTestFlac != NULL) {
    // TagLib's default behaviour treats Description as Comment
    if (fileTestFlac->xiphComment()->contains("COMMENT")) {
      TagLib::String out = fileTestFlac->xiphComment()->fieldListMap()["COMMENT"].toString();
      return QString::fromUtf8((out.toCString(true)));
    } else {
      return "";
    }
  }

  TagLib::String out = f->tag()->comment();
  return QString::fromUtf8(out.toCString(true));
}

QString TagLibMetadata::getGrouping() const {

  if (f == NULL || !f->isValid())
    return GuiStrings::getInstance()->notApplicable();

  TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
  if (fileTestMpeg != NULL) {
    return getGroupingId3(fileTestMpeg->ID3v2Tag());
  }

  TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
  if (fileTestAiff != NULL) {
    return getGroupingId3(fileTestAiff->tag());
  }

  TagLib::RIFF::WAV::File* fileTestWav = dynamic_cast<TagLib::RIFF::WAV::File*>(f);
  if (fileTestWav != NULL) {
    return getGroupingId3(fileTestWav->tag());
  }

  TagLib::MP4::Tag* tagTestMp4 = dynamic_cast<TagLib::MP4::Tag*>(f->tag());
  if (tagTestMp4 != NULL) {
    TagLib::MP4::Item m = tagTestMp4->itemListMap()["\251grp"];
    if (!m.isValid()) return "";
    TagLib::String out = m.toStringList().front();
    return QString::fromUtf8((out.toCString(true)));
  }

  TagLib::ASF::Tag* tagTestAsf = dynamic_cast<TagLib::ASF::Tag*>(f->tag());
  if (tagTestAsf != NULL) {
    TagLib::ASF::AttributeList l = tagTestAsf->attributeListMap()["WM/ContentGroupDescription"];
    if (l.isEmpty()) return "";
    TagLib::String out = l.front().toString();
    return QString::fromUtf8((out.toCString(true)));
  }

  TagLib::APE::Tag* tagTestApe = dynamic_cast<TagLib::APE::Tag*>(f->tag());
  if (tagTestApe != NULL) {
    TagLib::APE::Item m = tagTestApe->itemListMap()["Grouping"];
    if (m.isEmpty()) return "";
    TagLib::String out = m.toStringList().front();
    return QString::fromUtf8((out.toCString(true)));
  }

  return GuiStrings::getInstance()->notApplicable();
}

QString TagLibMetadata::getGroupingId3(const TagLib::ID3v2::Tag* tag) const {
  if (tag->isEmpty()) // ID3v1 doesn't support the Grouping tag
    return GuiStrings::getInstance()->notApplicable();
  TagLib::ID3v2::FrameList l = tag->frameListMap()["TIT1"];
  if (l.isEmpty()) return "";
  TagLib::String out = l.front()->toString();
  return QString::fromUtf8((out.toCString(true)));
}

QString TagLibMetadata::getKey() const {

  if (f == NULL || !f->isValid())
    return GuiStrings::getInstance()->notApplicable();

  TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
  if (fileTestMpeg != NULL) {
    return getKeyId3(fileTestMpeg->ID3v2Tag());
  }

  TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
  if (fileTestAiff != NULL) {
    return getKeyId3(fileTestAiff->tag());
  }

  TagLib::RIFF::WAV::File* fileTestWav = dynamic_cast<TagLib::RIFF::WAV::File*>(f);
  if (fileTestWav != NULL) {
    return getKeyId3(fileTestWav->tag());
  }

  TagLib::MP4::Tag* tagTestMp4 = dynamic_cast<TagLib::MP4::Tag*>(f->tag());
  if (tagTestMp4 != NULL) {
    TagLib::MP4::Item m = tagTestMp4->itemListMap()["----:com.apple.iTunes:initialkey"];
    if (!m.isValid()) return "";
    TagLib::String out = m.toStringList().front();
    return QString::fromUtf8((out.toCString(true)));
  }

  TagLib::ASF::Tag* tagTestAsf = dynamic_cast<TagLib::ASF::Tag*>(f->tag());
  if (tagTestAsf != NULL) {
    TagLib::ASF::AttributeList l = tagTestAsf->attributeListMap()["WM/InitialKey"];
    if (l.isEmpty()) return "";
    TagLib::String out = l.front().toString();
    return QString::fromUtf8((out.toCString(true)));
  }

  TagLib::FLAC::File* fileTestFlac = dynamic_cast<TagLib::FLAC::File*>(f);
  if (fileTestFlac != NULL) {
    if (!fileTestFlac->xiphComment()->contains("INITIALKEY")) return "";
    TagLib::String out = fileTestFlac->xiphComment()->fieldListMap()["INITIALKEY"].toString();
    return QString::fromUtf8((out.toCString(true)));
  }

  TagLib::APE::Tag* tagTestApe = dynamic_cast<TagLib::APE::Tag*>(f->tag());
  if (tagTestApe != NULL) {
    return GuiStrings::getInstance()->notApplicable();
  }

  return GuiStrings::getInstance()->notApplicable();
}

QString TagLibMetadata::getKeyId3(const TagLib::ID3v2::Tag* tag) const {
  if (tag->isEmpty()) // ID3v1 doesn't support the Key tag
    return GuiStrings::getInstance()->notApplicable();
  TagLib::ID3v2::FrameList l = tag->frameListMap()["TKEY"];
  if (l.isEmpty()) return "";
  TagLib::String out = l.front()->toString();
  return QString::fromUtf8((out.toCString(true)));
}

MetadataWriteResult TagLibMetadata::writeKeyToMetadata(KeyFinder::key_t key, const Preferences& prefs) {
  MetadataWriteResult result;
  QString data = prefs.getKeyCode(key);
  QString empty;
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    result.newTags.push_back(empty);
    if ((metadata_tag_t)i == METADATA_TAG_KEY) {
      writeKeyByTagEnum(data.left(3), (metadata_tag_t)i, result, prefs); // Key field in ID3 holds only 3 chars
    } else {
      writeKeyByTagEnum(data, (metadata_tag_t)i, result, prefs);
    }
  }
  return result;
}

void TagLibMetadata::writeKeyByTagEnum(const QString& data, metadata_tag_t tag, MetadataWriteResult& result, const Preferences& prefs) {
  QString delim = prefs.getMetadataDelimiter();
  metadata_write_t write = prefs.getMetadataWriteByTagEnum(tag);
  if (write == METADATA_WRITE_OVERWRITE) {
    if (getByTagEnum(tag) != data && setByTagEnum(data, tag))
      result.newTags[tag] = data;
  } else if (write == METADATA_WRITE_PREPEND) {
    QString current = getByTagEnum(tag);
    QString newData = (current.isEmpty() ? data : data + delim + current);
    if (current.left(data.length()) != data && setByTagEnum(newData, tag))
      result.newTags[tag] = newData ;
  } else if (write == METADATA_WRITE_APPEND) {
    QString current = getByTagEnum(tag);
    QString newData = (current.isEmpty() ? data : current + delim + data);
    if (current.right(data.length()) != data && setByTagEnum(newData, tag))
      result.newTags[tag] = newData;
  }
}

bool TagLibMetadata::setByTagEnum(const QString& data, metadata_tag_t tag) {
  if      (tag == METADATA_TAG_TITLE)    return setTitle(data);
  else if (tag == METADATA_TAG_ARTIST)   return setArtist(data);
  else if (tag == METADATA_TAG_ALBUM)    return setAlbum(data);
  else if (tag == METADATA_TAG_COMMENT)  return setComment(data);
  else if (tag == METADATA_TAG_GROUPING) return setGrouping(data);
  else if (tag == METADATA_TAG_KEY)      return setKey(data);
  return false;
}

bool TagLibMetadata::setTitle(const QString& tit) {
  if (f == NULL || !f->isValid()) return false;
  f->tag()->setTitle(TagLib::String(tit.toUtf8().constData(), TagLib::String::UTF8));
  f->save();
  return true;
}

bool TagLibMetadata::setArtist(const QString& art) {
  if (f == NULL || !f->isValid()) return false;
  f->tag()->setArtist(TagLib::String(art.toUtf8().constData(), TagLib::String::UTF8));
  f->save();
  return true;
}

bool TagLibMetadata::setAlbum(const QString& alb) {
  if (f == NULL || !f->isValid()) return false;
  f->tag()->setAlbum(TagLib::String(alb.toUtf8().constData(), TagLib::String::UTF8));
  f->save();
  return true;
}

bool TagLibMetadata::setComment(const QString& cmt) {
  if (f == NULL || !f->isValid()) return false;

  // TagLib's default behaviour for FLACs treats Description as Comment
  TagLib::FLAC::File* fileTestFlac = dynamic_cast<TagLib::FLAC::File*>(f);
  if (fileTestFlac != NULL) {
    fileTestFlac->xiphComment()->addField("COMMENT",TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8),true);
    f->save();
    return true;
  }

  // TagLib's default behaviour will write a v2 ID3 tag where none exists
  TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
  if (fileTestMpeg != NULL && fileTestMpeg->ID3v2Tag()->isEmpty()) {
    fileTestMpeg->ID3v1Tag()->setComment(TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8));
    fileTestMpeg->save(TagLib::MPEG::File::ID3v1, false);
    return true;
  }

  // non-FLAC behaviour
  f->tag()->setComment(TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8));

  // iTunes hack for ID3v2 in MPEGs and AIFFs, but iTunes doesn't read WAV tags
  TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
  if (fileTestMpeg != NULL && !fileTestMpeg->ID3v2Tag()->isEmpty()) {
    setITunesCommentId3v2(fileTestMpeg->ID3v2Tag(), cmt);
    fileTestMpeg->save(TagLib::MPEG::File::ID3v2, false, fileTestMpeg->ID3v2Tag()->header()->majorVersion());
  } else if (fileTestAiff != NULL) {
    setITunesCommentId3v2(fileTestAiff->tag(), cmt);
    f->save();
  } else {
    f->save();
  }
  return true;
}

void TagLibMetadata::setITunesCommentId3v2(TagLib::ID3v2::Tag* tag, const QString& cmt) {
  const TagLib::ID3v2::FrameList &comments = tag->frameListMap()["COMM"];
  bool done = false;
  for (TagLib::ID3v2::FrameList::ConstIterator it = comments.begin(); it != comments.end(); it++) {
    // overwrite all appropriate comment elements
    TagLib::ID3v2::CommentsFrame *commFrame = dynamic_cast<TagLib::ID3v2::CommentsFrame *>(*it);
    if (commFrame && commFrame->description().isEmpty()) {
      // TODO does this have implications for localisation?
      commFrame->setLanguage("eng");
      commFrame->setText(TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8));
      // we don't save here, because MPEGs need v2.3 / 2.4 handling.
      done = true;
    }
  }
  if (done) return;
  TagLib::ID3v2::CommentsFrame* frm = new TagLib::ID3v2::CommentsFrame();
  frm->setText(TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8));
  frm->setLanguage("eng");
  tag->addFrame(frm);
  // again, don't save here.
  return;
}

bool TagLibMetadata::setGrouping(const QString& grp) {
  if (f == NULL || !f->isValid()) return false;

  TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
  if (fileTestMpeg != NULL && setGroupingId3(fileTestMpeg->ID3v2Tag(), grp)) {
    fileTestMpeg->save(TagLib::MPEG::File::AllTags, true, fileTestMpeg->ID3v2Tag()->header()->majorVersion());
    return true;
  }

  TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
  if (fileTestAiff != NULL && setGroupingId3(fileTestAiff->tag(), grp)) {
    f->save();
    return true;
  }

  TagLib::RIFF::WAV::File* fileTestWav = dynamic_cast<TagLib::RIFF::WAV::File*>(f);
  if (fileTestWav != NULL && setGroupingId3(fileTestWav->tag(), grp)) {
    f->save();
    return true;
  }

  TagLib::MP4::Tag* tagTestMp4 = dynamic_cast<TagLib::MP4::Tag*>(f->tag());
  if (tagTestMp4 != NULL) {
    TagLib::StringList sl(TagLib::String(grp.toUtf8().constData(), TagLib::String::UTF8));
    tagTestMp4->itemListMap()["\251grp"] = sl;
    f->save();
    return true;
  }

  TagLib::ASF::Tag* tagTestAsf = dynamic_cast<TagLib::ASF::Tag*>(f->tag());
  if (tagTestAsf != NULL) {
    tagTestAsf->setAttribute("WM/ContentGroupDescription",TagLib::String(grp.toUtf8().constData(), TagLib::String::UTF8));
    f->save();
    return true;
  }

  TagLib::APE::Tag* tagTestApe = dynamic_cast<TagLib::APE::Tag*>(f->tag());
  if (tagTestApe != NULL) {
    tagTestApe->addValue("GROUPING",TagLib::String(grp.toUtf8().constData(), TagLib::String::UTF8));
    f->save();
    return true;
  }

  return false;
}

bool TagLibMetadata::setGroupingId3(TagLib::ID3v2::Tag* tag, const QString& grp) {
  if (tag->isEmpty()) return false; // ID3v1 doesn't support Grouping
  TagLib::ID3v2::Frame* frm = new TagLib::ID3v2::TextIdentificationFrame("TIT1");
  frm->setText(TagLib::String(grp.toUtf8().constData(), TagLib::String::UTF8));
  tag->removeFrames("TIT1");
  tag->addFrame(frm);
  // again, don't save here
  return true;
}

bool TagLibMetadata::setKey(const QString& key) {
  if (f == NULL || !f->isValid()) return false;

  TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
  if (fileTestMpeg != NULL && setKeyId3(fileTestMpeg->ID3v2Tag(), key)) {
    fileTestMpeg->save(TagLib::MPEG::File::ID3v2, false, fileTestMpeg->ID3v2Tag()->header()->majorVersion());
    return true;
  }

  TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
  if (fileTestAiff != NULL && setKeyId3(fileTestAiff->tag(), key)) {
    f->save();
    return true;
  }

  TagLib::RIFF::WAV::File* fileTestWav = dynamic_cast<TagLib::RIFF::WAV::File*>(f);
  if (fileTestWav != NULL && setKeyId3(fileTestWav->tag(), key)) {
    f->save();
    return true;
  }

  TagLib::MP4::Tag* tagTestMp4 = dynamic_cast<TagLib::MP4::Tag*>(f->tag());
  if (tagTestMp4 != NULL) {
    TagLib::StringList sl(TagLib::String(key.toUtf8().constData(), TagLib::String::UTF8));
    tagTestMp4->itemListMap()["----:com.apple.iTunes:initialkey"] = sl;
    f->save();
    return true;
  }

  TagLib::ASF::Tag* tagTestAsf = dynamic_cast<TagLib::ASF::Tag*>(f->tag());
  if (tagTestAsf != NULL) {
    tagTestAsf->setAttribute("WM/InitialKey",TagLib::String(key.toUtf8().constData(), TagLib::String::UTF8));
    f->save();
    return true;
  }

  TagLib::FLAC::File* fileTestFlac = dynamic_cast<TagLib::FLAC::File*>(f);
  if (fileTestFlac != NULL) {
    fileTestFlac->xiphComment()->addField("INITIALKEY",TagLib::String(key.toUtf8().constData(), TagLib::String::UTF8),true);
    f->save();
    return true;
  }

  TagLib::APE::Tag* tagTestApe = dynamic_cast<TagLib::APE::Tag*>(f->tag());
  if (tagTestApe != NULL) {
    return false;
  }

  return false;
}

bool TagLibMetadata::setKeyId3(TagLib::ID3v2::Tag* tag, const QString& key) {
  if (tag->isEmpty()) return false; // ID3v1 doesn't support Key
  TagLib::ID3v2::Frame* frm = new TagLib::ID3v2::TextIdentificationFrame("TKEY");
  frm->setText(TagLib::String(key.toUtf8().constData(), TagLib::String::UTF8));
  tag->removeFrames("TKEY");
  tag->addFrame(frm);
  // again, don't save in here
  return true;
}
