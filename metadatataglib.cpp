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

TagLibMetadata::TagLibMetadata(const QString& filePath){

  QMutexLocker locker(&constructor_mutex); // mutex the constructor

  QString fileExt = filePath.mid(filePath.lastIndexOf(".") + 1).toLower();

#ifdef Q_OS_WIN
  // Using utf16_to_utf8 here, as per decoderlibav, leads to a null file reference.
  const wchar_t* filePathCh = reinterpret_cast<const wchar_t*>(filePath.constData());
#else
  QByteArray encodedPath = QFile::encodeName(filePath);
  const char* filePathCh = encodedPath;
#endif

  f = NULL;

  if(fileExt == "mp3")
    f = new TagLib::MPEG::File(filePathCh);
  if(fileExt == "aif" || fileExt == "aiff")
    f = new TagLib::RIFF::AIFF::File(filePathCh);
  if(fileExt == "wav")
    f = new TagLib::RIFF::WAV::File(filePathCh);
  if(fileExt == "ogg")
    f = new TagLib::Ogg::Vorbis::File(filePathCh);
  if(fileExt == "oga"){
    f = new TagLib::Ogg::FLAC::File(filePathCh);
    if (f == NULL || !f->isValid()){
      delete f;
      f = new TagLib::Ogg::Vorbis::File(filePathCh);
    }
  }
  if(fileExt == "flac")
    f = new TagLib::FLAC::File(filePathCh);
  //  if(fileExt == "ape") f = new TagLib::APE::File(filePathCh);
  if(fileExt == "mpc")
    f = new TagLib::MPC::File(filePathCh);
  if(fileExt == "wv")
    f = new TagLib::WavPack::File(filePathCh);
  if(fileExt == "spx")
    f = new TagLib::Ogg::Speex::File(filePathCh);
  if(fileExt == "tta")
    f = new TagLib::TrueAudio::File(filePathCh);
#ifdef TAGLIB_WITH_MP4
  if(fileExt == "m4a" || fileExt == "m4b" || fileExt == "m4p" || fileExt == "mp4" || fileExt == "3g2")
    f = new TagLib::MP4::File(filePathCh);
#endif
#ifdef TAGLIB_WITH_ASF
  if(fileExt == "wma" || fileExt == "asf")
    f = new TagLib::ASF::File(filePathCh);
#endif

  locker.~QMutexLocker(); // unlock mutex

  if(f != NULL && f->isValid())
    return; // everything's fine.

  // or else...
  f = NULL;
#ifdef Q_OS_WIN
  qDebug("TagLib returned NULL File for %s", utf16_to_utf8(filePathCh));
#else
  qDebug("TagLib returned NULL File for %s", filePathCh);
#endif
  return;
}

TagLibMetadata::~TagLibMetadata(){
  if(f != NULL)
    delete f;
}

QString TagLibMetadata::getTitle() const{
  if(f == NULL || !f->isValid())
    return GuiStrings::getInstance()->notApplicable();
  TagLib::String out = f->tag()->title();
  return QString::fromUtf8(out.toCString(true));
}

QString TagLibMetadata::getArtist() const{
  if(f == NULL || !f->isValid())
    return GuiStrings::getInstance()->notApplicable();
  TagLib::String out = f->tag()->artist();
  return QString::fromUtf8(out.toCString(true));
}

QString TagLibMetadata::getComment() const{
  if(f == NULL || !f->isValid())
    return GuiStrings::getInstance()->notApplicable();
  TagLib::FLAC::File* fileTestFlac = dynamic_cast<TagLib::FLAC::File*>(f);
  if(fileTestFlac != NULL){
    // TagLib's default behaviour treats Description as Comment: override
    if(fileTestFlac->xiphComment()->contains("COMMENT")){
      TagLib::String out = fileTestFlac->xiphComment()->fieldListMap()["COMMENT"].toString();
      return QString::fromUtf8((out.toCString(true)));
    }else{
      return "";
    }
  }

  TagLib::String out = f->tag()->comment();
  return QString::fromUtf8(out.toCString(true));
}

QString TagLibMetadata::getGrouping() const{

  if(f == NULL || !f->isValid())
    return GuiStrings::getInstance()->notApplicable();

  TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
  if(fileTestMpeg != NULL){
    return getGroupingId3(fileTestMpeg->ID3v2Tag());
  }

  TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
  if(fileTestAiff != NULL){
    return getGroupingId3(fileTestAiff->tag());
  }

  TagLib::RIFF::WAV::File* fileTestWav = dynamic_cast<TagLib::RIFF::WAV::File*>(f);
  if(fileTestWav != NULL){
    return getGroupingId3(fileTestWav->tag());
  }

  TagLib::MP4::Tag* tagTestMp4 = dynamic_cast<TagLib::MP4::Tag*>(f->tag());
  if(tagTestMp4 != NULL){
    TagLib::MP4::Item m = tagTestMp4->itemListMap()["\251grp"];
    if(m.isValid()){
      TagLib::String out = m.toStringList().front();
      return QString::fromUtf8((out.toCString(true)));
    }
    return "";
  }

  TagLib::ASF::Tag* tagTestAsf = dynamic_cast<TagLib::ASF::Tag*>(f->tag());
  if(tagTestAsf != NULL){
    TagLib::ASF::AttributeList l = tagTestAsf->attributeListMap()["WM/ContentGroupDescription"];
    if(!l.isEmpty()){
      TagLib::String out = l.front().toString();
      return QString::fromUtf8((out.toCString(true)));
    }
    return "";
  }

  TagLib::APE::Tag* tagTestApe = dynamic_cast<TagLib::APE::Tag*>(f->tag());
  if(tagTestApe != NULL){
    TagLib::APE::Item m = tagTestApe->itemListMap()["Grouping"];
    if(!m.isEmpty()){
      TagLib::String out = m.toStringList().front();
      return QString::fromUtf8((out.toCString(true)));
    }
    return "";
  }

  return GuiStrings::getInstance()->notApplicable();
}

QString TagLibMetadata::getGroupingId3(const TagLib::ID3v2::Tag* tag) const{
  if(tag != NULL){
    TagLib::ID3v2::FrameList l = tag->frameListMap()["TIT1"];
    if(!l.isEmpty()){
      TagLib::String out = l.front()->toString();
      return QString::fromUtf8((out.toCString(true)));
    }
    return "";
  }
  // ID3v1 doesn't support the Grouping tag
  return GuiStrings::getInstance()->notApplicable();
}

QString TagLibMetadata::getKey() const{

  if(f == NULL || !f->isValid())
    return GuiStrings::getInstance()->notApplicable();

  TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
  if(fileTestMpeg != NULL){
    return getKeyId3(fileTestMpeg->ID3v2Tag());
  }

  TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
  if(fileTestAiff != NULL){
    return getKeyId3(fileTestAiff->tag());
  }

  TagLib::RIFF::WAV::File* fileTestWav = dynamic_cast<TagLib::RIFF::WAV::File*>(f);
  if(fileTestWav != NULL){
    return getKeyId3(fileTestWav->tag());
  }

  TagLib::MP4::Tag* tagTestMp4 = dynamic_cast<TagLib::MP4::Tag*>(f->tag());
  if(tagTestMp4 != NULL){
    TagLib::MP4::Item m = tagTestMp4->itemListMap()["----:com.apple.iTunes:initialkey"];
    if(m.isValid()){
      TagLib::String out = m.toStringList().front();
      return QString::fromUtf8((out.toCString(true)));
    }
    return "";
  }

  TagLib::ASF::Tag* tagTestAsf = dynamic_cast<TagLib::ASF::Tag*>(f->tag());
  if(tagTestAsf != NULL){
    TagLib::ASF::AttributeList l = tagTestAsf->attributeListMap()["WM/InitialKey"];
    if(!l.isEmpty()){
      TagLib::String out = l.front().toString();
      return QString::fromUtf8((out.toCString(true)));
    }
    return "";
  }

  TagLib::FLAC::File* fileTestFlac = dynamic_cast<TagLib::FLAC::File*>(f);
  if(fileTestFlac != NULL){
    if(fileTestFlac->xiphComment()->contains("INITIALKEY")){
      TagLib::String out = fileTestFlac->xiphComment()->fieldListMap()["INITIALKEY"].toString();
      return QString::fromUtf8((out.toCString(true)));
    }else{
      return "";
    }
  }

  TagLib::APE::Tag* tagTestApe = dynamic_cast<TagLib::APE::Tag*>(f->tag());
  if(tagTestApe != NULL){
    return GuiStrings::getInstance()->notApplicable();
  }

  return GuiStrings::getInstance()->notApplicable();
}

QString TagLibMetadata::getKeyId3(const TagLib::ID3v2::Tag* tag) const{
  if(tag != NULL){
    TagLib::ID3v2::FrameList l = tag->frameListMap()["TKEY"];
    if(!l.isEmpty()){
      TagLib::String out = l.front()->toString();
      return QString::fromUtf8((out.toCString(true)));
    }
    return "";
  }
  // ID3v1 doesn't support the Key tag
  return GuiStrings::getInstance()->notApplicable();
}

QString TagLibMetadata::writeKeyToMetadata(int key, const Preferences& prefs){

  QString result = "";

  QString dataToWrite = prefs.getKeyCode(key);
  QString delim = prefs.getMetadataDelimiter();

  if(prefs.getMetadataWriteComment() == METADATA_WRITE_OVERWRITE){
    if(getComment() != dataToWrite && setComment(dataToWrite))
      result += "c";
  }else if(prefs.getMetadataWriteComment() == METADATA_WRITE_PREPEND){
    if(getComment().left(dataToWrite.length()) != dataToWrite && setComment(dataToWrite + delim + getComment()))
      result += "c";
  }else if(prefs.getMetadataWriteComment() == METADATA_WRITE_APPEND){
    if(getComment().right(dataToWrite.length()) != dataToWrite && setComment(getComment() + delim + dataToWrite))
      result += "c";
  }

  if(prefs.getMetadataWriteGrouping() == METADATA_WRITE_OVERWRITE){
    if(getGrouping() != dataToWrite && setGrouping(dataToWrite))
      result += "g";
  }else if(prefs.getMetadataWriteGrouping() == METADATA_WRITE_PREPEND){
    if(getGrouping().left(dataToWrite.length()) != dataToWrite && setGrouping(dataToWrite + delim + getGrouping()))
      result += "g";
  }else if(prefs.getMetadataWriteGrouping() == METADATA_WRITE_APPEND){
    if(getGrouping().right(dataToWrite.length()) != dataToWrite && setGrouping(getGrouping() + delim + dataToWrite))
      result += "g";
  }

  dataToWrite = dataToWrite.left(3); // Key field in ID3 holds only 3 chars
  if(prefs.getMetadataWriteKey() == METADATA_WRITE_OVERWRITE)
    if(getKey() != dataToWrite && setKey(dataToWrite))
        result += "k";

  return result;

}

bool TagLibMetadata::setComment(const QString& cmt){
  if(f == NULL || !f->isValid()){
    return false;
  }

  // TagLib's default behaviour for FLACs treats Description as Comment. Override.
  TagLib::FLAC::File* fileTestFlac = dynamic_cast<TagLib::FLAC::File*>(f);
  if(fileTestFlac != NULL){
    fileTestFlac->xiphComment()->addField("COMMENT",TagLib::String(cmt.toLocal8Bit().constData()),true);
    f->save();
    return true;
  }

  // non-FLAC behaviour
  f->tag()->setComment(TagLib::String(cmt.toLocal8Bit().constData()));
  f->save();

  // iTunes hack for MPEGs and AIFFs, but iTunes doesn't read WAV tags
  TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
  if(fileTestMpeg != NULL){
    setITunesCommentId3(fileTestMpeg->ID3v2Tag(), cmt);
  }
  TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
  if(fileTestAiff != NULL){
    setITunesCommentId3(fileTestAiff->tag(), cmt);
  }
  return true;
}

void TagLibMetadata::setITunesCommentId3(TagLib::ID3v2::Tag* tag, const QString& cmt){
  if(tag != NULL){
    const TagLib::ID3v2::FrameList &comments = tag->frameListMap()["COMM"];
    bool done = false;
    for(TagLib::ID3v2::FrameList::ConstIterator it = comments.begin(); it != comments.end(); it++){
      // overwrite all appropriate comment elements
      TagLib::ID3v2::CommentsFrame *commFrame = dynamic_cast<TagLib::ID3v2::CommentsFrame *>(*it);
      if(commFrame && commFrame->description().isEmpty()){
        // TODO does this have implications for localisation?
        commFrame->setLanguage("eng");
        commFrame->setText(TagLib::String(cmt.toLocal8Bit().constData()));
        f->save();
        done = true;
      }
    }
    if(done) return;
    TagLib::ID3v2::CommentsFrame* frm = new TagLib::ID3v2::CommentsFrame();
    frm->setText(TagLib::String(cmt.toLocal8Bit().constData()));
    frm->setLanguage("eng");
    tag->addFrame(frm);
    f->save();
    return;
  }
}

bool TagLibMetadata::setGrouping(const QString& grp){

  if(f == NULL || !f->isValid()){
    return false;
  }

  TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
  if(fileTestMpeg != NULL){
    return setGroupingId3(fileTestMpeg->ID3v2Tag(), grp);
  }

  TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
  if(fileTestAiff != NULL){
    return setGroupingId3(fileTestAiff->tag(), grp);
  }

  TagLib::RIFF::WAV::File* fileTestWav = dynamic_cast<TagLib::RIFF::WAV::File*>(f);
  if(fileTestWav != NULL){
    return setGroupingId3(fileTestWav->tag(), grp);
  }

  TagLib::MP4::Tag* tagTestMp4 = dynamic_cast<TagLib::MP4::Tag*>(f->tag());
  if(tagTestMp4 != NULL){
    TagLib::StringList sl(TagLib::String(grp.toLocal8Bit().constData()));
    tagTestMp4->itemListMap()["\251grp"] = sl;
    tagTestMp4->save();
    f->save();
    return true;
  }

  TagLib::ASF::Tag* tagTestAsf = dynamic_cast<TagLib::ASF::Tag*>(f->tag());
  if(tagTestAsf != NULL){
    tagTestAsf->setAttribute("WM/ContentGroupDescription",TagLib::String(grp.toLocal8Bit().constData()));
    f->save();
    return true;
  }

  TagLib::APE::Tag* tagTestApe = dynamic_cast<TagLib::APE::Tag*>(f->tag());
  if(tagTestApe != NULL){
    tagTestApe->addValue("GROUPING",TagLib::String(grp.toLocal8Bit().constData()));
    f->save();
    return true;
  }

  return false;
}

bool TagLibMetadata::setGroupingId3(TagLib::ID3v2::Tag* tag, const QString& grp){
  if(tag != NULL){
    TagLib::ID3v2::Frame* frm = new TagLib::ID3v2::TextIdentificationFrame("TIT1");
    frm->setText(TagLib::String(grp.toLocal8Bit().constData()));
    tag->removeFrames("TIT1");
    tag->addFrame(frm);
    f->save();
    return true;
  }else{
    // ID3v1 doesn't support Grouping
    return false;
  }
}

bool TagLibMetadata::setKey(const QString& key){

  if(f == NULL || !f->isValid()){
    return false;
  }

  TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
  if(fileTestMpeg != NULL){
    return setKeyId3(fileTestMpeg->ID3v2Tag(), key);
  }

  TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
  if(fileTestAiff != NULL){
    return setKeyId3(fileTestAiff->tag(), key);
  }

  TagLib::RIFF::WAV::File* fileTestWav = dynamic_cast<TagLib::RIFF::WAV::File*>(f);
  if(fileTestWav != NULL){
    return setKeyId3(fileTestWav->tag(), key);
  }

  TagLib::MP4::Tag* tagTestMp4 = dynamic_cast<TagLib::MP4::Tag*>(f->tag());
  if(tagTestMp4 != NULL){
    TagLib::StringList sl(TagLib::String(key.toLocal8Bit().constData()));
    tagTestMp4->itemListMap()["----:com.apple.iTunes:initialkey"] = sl;
    tagTestMp4->save();
    f->save();
    return true;
  }

  TagLib::ASF::Tag* tagTestAsf = dynamic_cast<TagLib::ASF::Tag*>(f->tag());
  if(tagTestAsf != NULL){
    tagTestAsf->setAttribute("WM/InitialKey",TagLib::String(key.toLocal8Bit().constData()));
    f->save();
    return true;
  }

  TagLib::FLAC::File* fileTestFlac = dynamic_cast<TagLib::FLAC::File*>(f);
  if(fileTestFlac != NULL){
    fileTestFlac->xiphComment()->addField("INITIALKEY",TagLib::String(key.toLocal8Bit().constData()),true);
    f->save();
    return true;
  }

  TagLib::APE::Tag* tagTestApe = dynamic_cast<TagLib::APE::Tag*>(f->tag());
  if(tagTestApe != NULL){
    return false;
  }

  return false;
}

bool TagLibMetadata::setKeyId3(TagLib::ID3v2::Tag* tag, const QString& key){
  if(tag != NULL){
    TagLib::ID3v2::Frame* frm = new TagLib::ID3v2::TextIdentificationFrame("TKEY");
    frm->setText(TagLib::String(key.toLocal8Bit().constData()));
    tag->removeFrames("TKEY");
    tag->addFrame(frm);
    f->save();
    return true;
  }else{
    // ID3v1 doesn't support Key
    return false;
  }
}
