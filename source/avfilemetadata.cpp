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

#include "avfilemetadata.h"

const QString emptyString = "";
const char* keyXiphTagComment      = "COMMENT";
const char* keyId3TagiTunesComment = "COMM";
const char* lngId3TagiTunesComment = "eng"; // will this mess up localisations?
const char* keyMp4TagGrouping      = "\251grp";
const char* keyAsfTagGrouping      = "WM/ContentGroupDescription";
const char* keyApeTagGrouping      = "Grouping";
const char* keyId3TagGrouping      = "TIT1";
const char* keyId3TagKey           = "TKEY";
const char* keyMp4TagKey           = "----:com.apple.iTunes:initialkey";
const char* keyXiphTagKey          = "INITIALKEY";
const char* keyAsfTagKey           = "WM/InitialKey";

AVFileMetadata::AVFileMetadata(TagLib::FileRef* inFr, TagLib::File* f) : fr(inFr), genericFile(f) { }
NullFileMetadata::NullFileMetadata      (TagLib::FileRef* fr, TagLib::File* g)                              : AVFileMetadata     (fr, g)       { }
FlacFileMetadata::FlacFileMetadata      (TagLib::FileRef* fr, TagLib::File* g, TagLib::FLAC::File* s)       : AVFileMetadata     (fr, g)       { flacFile = s; }
MpegID3FileMetadata::MpegID3FileMetadata(TagLib::FileRef* fr, TagLib::File* g, TagLib::MPEG::File* s)       : AVFileMetadata     (fr, g)       { mpegFile = s; }
AiffID3FileMetadata::AiffID3FileMetadata(TagLib::FileRef* fr, TagLib::File* g, TagLib::RIFF::AIFF::File* s) : MpegID3FileMetadata(fr, g, NULL) { aiffFile = s; }
WavID3FileMetadata::WavID3FileMetadata  (TagLib::FileRef* fr, TagLib::File* g, TagLib::RIFF::WAV::File* s)  : AiffID3FileMetadata(fr, g, NULL) { wavFile = s; }
Mp4FileMetadata::Mp4FileMetadata        (TagLib::FileRef* fr, TagLib::File* g, TagLib::MP4::File* s)        : AVFileMetadata     (fr, g)       { mp4File = s; }
AsfFileMetadata::AsfFileMetadata        (TagLib::FileRef* fr, TagLib::File* g, TagLib::ASF::File* s)        : AVFileMetadata     (fr, g)       { asfFile = s; }

AVFileMetadata::~AVFileMetadata() { delete fr; }
NullFileMetadata::~NullFileMetadata() { }

// ================================= GENERIC ===================================

QString AVFileMetadata::getByTagEnum(metadata_tag_t tag) const {
  if      (tag == METADATA_TAG_TITLE)    return getTitle();
  else if (tag == METADATA_TAG_ARTIST)   return getArtist();
  else if (tag == METADATA_TAG_ALBUM)    return getAlbum();
  else if (tag == METADATA_TAG_COMMENT)  return getComment();
  else if (tag == METADATA_TAG_GROUPING) return getGrouping();
  else if (tag == METADATA_TAG_KEY)      return getKey();
  return emptyString;
}

QString AVFileMetadata::getTitle() const {
  TagLib::String out = genericFile->tag()->title();
  return QString::fromUtf8(out.toCString(true));
}

QString AVFileMetadata::getArtist() const {
  TagLib::String out = genericFile->tag()->artist();
  return QString::fromUtf8(out.toCString(true));
}

QString AVFileMetadata::getAlbum() const {
  TagLib::String out = genericFile->tag()->album();
  return QString::fromUtf8(out.toCString(true));
}

QString AVFileMetadata::getComment() const {
  TagLib::String out = genericFile->tag()->comment();
  return QString::fromUtf8(out.toCString(true));
}

QString AVFileMetadata::getGrouping() const {
  return GuiStrings::getInstance()->notApplicable();
}

QString AVFileMetadata::getKey() const {
  return GuiStrings::getInstance()->notApplicable();
}

MetadataWriteResult AVFileMetadata::writeKeyToMetadata(
  KeyFinder::key_t key,
  const Preferences& prefs
) {
  MetadataWriteResult result;
  QString data = prefs.getKeyCode(key);
  QString empty;
  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    result.newTags.push_back(empty);
    if ((metadata_tag_t)i == METADATA_TAG_KEY) {
      // Key field in ID3 holds only 3 chars; treat all Key fields as the same
      writeKeyByTagEnum(data.left(METADATA_CHARLIMIT_KEY), (metadata_tag_t)i, result, prefs);
    } else {
      writeKeyByTagEnum(data, (metadata_tag_t)i, result, prefs);
    }
  }
  return result;
}

void AVFileMetadata::writeKeyByTagEnum(
  const QString& data,
  metadata_tag_t tag,
  MetadataWriteResult& result,
  const Preferences& prefs
) {
  // Key field in ID3 holds only 3 chars; treat all Key fields as the same
  unsigned int charLimit = (tag == METADATA_TAG_KEY ? METADATA_CHARLIMIT_KEY : METADATA_CHARLIMIT_OTHERS);
  QString newString = prefs.newString(
    data, getByTagEnum(tag), charLimit, prefs.getMetadataWriteByTagEnum(tag)
  );
  if (newString != "" && setByTagEnum(newString, tag)) {
    result.newTags[tag] = newString;
  }
}

bool AVFileMetadata::setByTagEnum(const QString& data, metadata_tag_t tag) {
  if      (tag == METADATA_TAG_TITLE)    return setTitle(data);
  else if (tag == METADATA_TAG_ARTIST)   return setArtist(data);
  else if (tag == METADATA_TAG_ALBUM)    return setAlbum(data);
  else if (tag == METADATA_TAG_COMMENT)  return setComment(data);
  else if (tag == METADATA_TAG_GROUPING) return setGrouping(data);
  else if (tag == METADATA_TAG_KEY)      return setKey(data);
  return false;
}

bool AVFileMetadata::setTitle(const QString& tit) {
  genericFile->tag()->setTitle(
    TagLib::String(tit.toUtf8().constData(), TagLib::String::UTF8)
  );
  genericFile->save();
  return true;
}

bool AVFileMetadata::setArtist(const QString& art) {
  genericFile->tag()->setArtist(
    TagLib::String(art.toUtf8().constData(), TagLib::String::UTF8)
  );
  genericFile->save();
  return true;
}

bool AVFileMetadata::setAlbum(const QString& alb) {
  genericFile->tag()->setAlbum(
    TagLib::String(alb.toUtf8().constData(), TagLib::String::UTF8)
  );
  genericFile->save();
  return true;
}

bool AVFileMetadata::setComment(const QString& cmt) {
  genericFile->tag()->setComment(
    TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8)
  );
  genericFile->save();
  return true;
}

bool AVFileMetadata::setGrouping(const QString& /*grp*/) {
  return false;
}

bool AVFileMetadata::setKey(const QString& /*key*/) {
  return false;
}

// =================================== NULL ====================================

QString NullFileMetadata::getByTagEnum(metadata_tag_t /*tag*/) const {
  return GuiStrings::getInstance()->notApplicable();
}

QString NullFileMetadata::getTitle() const {
  return GuiStrings::getInstance()->notApplicable();
}

QString NullFileMetadata::getArtist() const {
  return GuiStrings::getInstance()->notApplicable();
}

QString NullFileMetadata::getAlbum() const {
  return GuiStrings::getInstance()->notApplicable();
}

QString NullFileMetadata::getComment() const {
  return GuiStrings::getInstance()->notApplicable();
}

bool NullFileMetadata::setTitle(const QString& /*tit*/) {
  return false;
}

bool NullFileMetadata::setArtist(const QString& /*tit*/) {
  return false;
}

bool NullFileMetadata::setAlbum(const QString& /*tit*/) {
  return false;
}

bool NullFileMetadata::setComment(const QString& /*cmt*/) {
  return false;
}

// =================================== FLAC ====================================

QString FlacFileMetadata::getComment() const {
  // TagLib's default behaviour treats Description as Comment
  if (flacFile->xiphComment()->contains(keyXiphTagComment)) {
    TagLib::String out = flacFile->xiphComment()->fieldListMap()[keyXiphTagComment].toString();
    return QString::fromUtf8((out.toCString(true)));
  } else {
    return emptyString;
  }
}

QString FlacFileMetadata::getKey() const {
  TagLib::Ogg::XiphComment* c = flacFile->xiphComment();
  if (!c->fieldListMap().contains(keyXiphTagKey))
    return emptyString;
  TagLib::String out = c->fieldListMap()[keyXiphTagKey].toString();
  return QString::fromUtf8((out.toCString(true)));
}

bool FlacFileMetadata::setComment(const QString& cmt) {
  // TagLib's default behaviour treats Description as Comment
  flacFile->xiphComment()->addField(
    keyXiphTagComment,
    TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8),
    true
  );
  genericFile->save();
  return true;
}

bool FlacFileMetadata::setKey(const QString& key) {
  flacFile->xiphComment()->addField(
    keyXiphTagKey,
    TagLib::String(key.toUtf8().constData(), TagLib::String::UTF8),
    true
  );
  flacFile->save();
  return true;
}

// =================================== MPEG ====================================

bool MpegID3FileMetadata::hasId3v1Tag() const {
  if (mpegFile == NULL) return false; // AIFF or WAV subclasses
  return !mpegFile->ID3v1Tag()->isEmpty();
}

bool MpegID3FileMetadata::hasId3v2Tag() const {
  if (mpegFile == NULL) return true; // AIFF or WAV subclasses
  return !mpegFile->ID3v2Tag()->isEmpty();
}

bool MpegID3FileMetadata::hasId3v2_3Tag() const {
  if (mpegFile == NULL) return true; // AIFF or WAV subclasses
  if (!hasId3v2Tag()) return false;
  if (mpegFile->ID3v2Tag()->header()->majorVersion() != 3) return false;
  return true;
}

bool MpegID3FileMetadata::hasId3v2_4Tag() const {
  if (mpegFile == NULL) return true; // AIFF or WAV subclasses
  if (!hasId3v2Tag()) return false;
  if (mpegFile->ID3v2Tag()->header()->majorVersion() != 4) return false;
  return true;
}

QString MpegID3FileMetadata::getGrouping() const {
  if (!hasId3v2Tag()) return GuiStrings::getInstance()->notApplicable();
  return getGroupingId3(mpegFile->ID3v2Tag());
}

QString MpegID3FileMetadata::getGroupingId3(const TagLib::ID3v2::Tag* tag) const {
  if (!tag->frameListMap().contains(keyId3TagGrouping))
    return emptyString;
  TagLib::ID3v2::FrameList l = tag->frameListMap()[keyId3TagGrouping];
  TagLib::String out = l.front()->toString();
  return QString::fromUtf8((out.toCString(true)));
}

QString MpegID3FileMetadata::getKey() const {
  if (!hasId3v2Tag()) return GuiStrings::getInstance()->notApplicable();
  return getKeyId3(mpegFile->ID3v2Tag());
}

QString MpegID3FileMetadata::getKeyId3(const TagLib::ID3v2::Tag* tag) const {
  if (!tag->frameListMap().contains(keyId3TagKey))
    return emptyString;
  TagLib::ID3v2::FrameList l = tag->frameListMap()[keyId3TagKey];
  TagLib::String out = l.front()->toString();
  return QString::fromUtf8((out.toCString(true)));
}

bool MpegID3FileMetadata::setTitle(const QString& tit) {
  bool written = false;
  if (hasId3v1Tag()) {
    // TagLib's default save behaviour will write a v2 ID3 tag where none exists
    mpegFile->ID3v1Tag()->setTitle(
      TagLib::String(tit.toUtf8().constData(), TagLib::String::UTF8)
    );
    mpegFile->save(TagLib::MPEG::File::ID3v1, false);
    written = true;
  }
  if (hasId3v2Tag()) {
    mpegFile->ID3v2Tag()->setTitle(
      TagLib::String(tit.toUtf8().constData(), TagLib::String::UTF8)
    );
    mpegFile->save(
      TagLib::MPEG::File::ID3v2, false,
      mpegFile->ID3v2Tag()->header()->majorVersion()
    );
    written = true;
  }
  return written;
}

bool MpegID3FileMetadata::setArtist(const QString& art) {
  bool written = false;
  if (hasId3v1Tag()) {
    mpegFile->ID3v1Tag()->setArtist(
      TagLib::String(art.toUtf8().constData(), TagLib::String::UTF8)
    );
    mpegFile->save(TagLib::MPEG::File::ID3v1, false);
    written = true;
  }
  if (hasId3v2Tag()) {
    mpegFile->ID3v2Tag()->setArtist(
      TagLib::String(art.toUtf8().constData(), TagLib::String::UTF8)
    );
    mpegFile->save(
      TagLib::MPEG::File::ID3v2, false,
      mpegFile->ID3v2Tag()->header()->majorVersion()
    );
    written = true;
  }
  return written;
}

bool MpegID3FileMetadata::setAlbum(const QString& alb) {
  bool written = false;
  if (hasId3v1Tag()) {
    mpegFile->ID3v1Tag()->setAlbum(
      TagLib::String(alb.toUtf8().constData(), TagLib::String::UTF8)
    );
    mpegFile->save(TagLib::MPEG::File::ID3v1, false);
    written = true;
  }
  if (hasId3v2Tag()) {
    mpegFile->ID3v2Tag()->setAlbum(
      TagLib::String(alb.toUtf8().constData(), TagLib::String::UTF8)
    );
    mpegFile->save(
      TagLib::MPEG::File::ID3v2, false,
      mpegFile->ID3v2Tag()->header()->majorVersion()
    );
    written = true;
  }
  return written;
}

bool MpegID3FileMetadata::setComment(const QString& cmt) {
  bool written = false;
  if (hasId3v1Tag()) {
    mpegFile->ID3v1Tag()->setComment(
      TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8)
    );
    mpegFile->save(TagLib::MPEG::File::ID3v1, false);
    written = true;
  }
  if (hasId3v2Tag()) {
    // basic tag
    mpegFile->ID3v2Tag()->setComment(
      TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8)
    );
    mpegFile->save(
      TagLib::MPEG::File::ID3v2, false,
      mpegFile->ID3v2Tag()->header()->majorVersion()
    );
    // iTunes comment hack
    setITunesCommentId3(mpegFile->ID3v2Tag(), cmt);
    mpegFile->save(
      TagLib::MPEG::File::ID3v2,
      false,
      mpegFile->ID3v2Tag()->header()->majorVersion()
    );
    written = true;
  }
  return written;
}

void MpegID3FileMetadata::setITunesCommentId3(TagLib::ID3v2::Tag* tag, const QString& cmt) {
  if (tag->frameListMap().contains(keyId3TagiTunesComment)) {
    const TagLib::ID3v2::FrameList &comments = tag->frameListMap()[keyId3TagiTunesComment];
    bool found = false;
    for (TagLib::ID3v2::FrameList::ConstIterator it = comments.begin(); it != comments.end(); it++) {
      // overwrite all appropriate comment elements
      TagLib::ID3v2::CommentsFrame *commFrame = dynamic_cast<TagLib::ID3v2::CommentsFrame *>(*it);
      if (commFrame && commFrame->description().isEmpty()) {
        commFrame->setLanguage(lngId3TagiTunesComment);
        commFrame->setText(TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8));
        // we don't save here, because MPEGs need v2.3 / 2.4 handling.
        found = true;
      }
    }
    if (found) return;
  }
  TagLib::ID3v2::CommentsFrame* frm = new TagLib::ID3v2::CommentsFrame();
  frm->setText(TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8));
  frm->setLanguage(lngId3TagiTunesComment);
  tag->addFrame(frm);
  // again, don't save here.
  return;
}

bool MpegID3FileMetadata::setGrouping(const QString& grp) {
  if (!hasId3v2Tag()) return false; // ID3v1 doesn't support Grouping
  setGroupingId3(mpegFile->ID3v2Tag(), grp);
  mpegFile->save(
    TagLib::MPEG::File::ID3v2, true,
    mpegFile->ID3v2Tag()->header()->majorVersion()
  );
  return true;
}

bool MpegID3FileMetadata::setGroupingId3(TagLib::ID3v2::Tag* tag, const QString& grp) {
  TagLib::ID3v2::Frame* frm = new TagLib::ID3v2::TextIdentificationFrame(keyId3TagGrouping);
  frm->setText(TagLib::String(grp.toUtf8().constData(), TagLib::String::UTF8));
  tag->removeFrames(keyId3TagGrouping);
  tag->addFrame(frm);
  // again, don't save here
  return true;
}

bool MpegID3FileMetadata::setKey(const QString& key) {
  if (!hasId3v2Tag()) return false; // ID3v1 doesn't support Key
  setKeyId3(mpegFile->ID3v2Tag(), key);
  mpegFile->save(
    TagLib::MPEG::File::ID3v2, false,
    mpegFile->ID3v2Tag()->header()->majorVersion()
  );
  return true;
}

bool MpegID3FileMetadata::setKeyId3(TagLib::ID3v2::Tag* tag, const QString& key) {
  TagLib::ID3v2::Frame* frm = new TagLib::ID3v2::TextIdentificationFrame(keyId3TagKey);
  frm->setText(TagLib::String(key.toUtf8().constData(), TagLib::String::UTF8));
  tag->removeFrames(keyId3TagKey);
  tag->addFrame(frm);
  // again, don't save in here
  return true;
}

// =================================== AIFF ====================================

QString AiffID3FileMetadata::getGrouping() const {
  return getGroupingId3(aiffFile->tag());
}

QString AiffID3FileMetadata::getKey() const {
  return getKeyId3(aiffFile->tag());
}

bool AiffID3FileMetadata::setTitle(const QString& tit) {
  aiffFile->tag()->setTitle(
    TagLib::String(tit.toUtf8().constData(), TagLib::String::UTF8)
  );
  aiffFile->save();
  return true;
}

bool AiffID3FileMetadata::setArtist(const QString& art) {
  aiffFile->tag()->setArtist(
    TagLib::String(art.toUtf8().constData(), TagLib::String::UTF8)
  );
  aiffFile->save();
  return true;
}

bool AiffID3FileMetadata::setAlbum(const QString& alb) {
  aiffFile->tag()->setAlbum(
    TagLib::String(alb.toUtf8().constData(), TagLib::String::UTF8)
  );
  aiffFile->save();
  return true;
}

bool AiffID3FileMetadata::setComment(const QString& cmt) {
  // basic tag
  genericFile->tag()->setComment(
    TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8)
  );
  // iTunes comment hack
  setITunesCommentId3(aiffFile->tag(), cmt);
  aiffFile->save();
  return true;
}

bool AiffID3FileMetadata::setGrouping(const QString& grp) {
  setGroupingId3(aiffFile->tag(), grp);
  aiffFile->save();
  return true;
}

bool AiffID3FileMetadata::setKey(const QString& key) {
  setKeyId3(aiffFile->tag(), key);
  aiffFile->save();
  return true;
}


// =================================== WAV =====================================

QString WavID3FileMetadata::getGrouping() const {
  return getGroupingId3(wavFile->tag());
}

QString WavID3FileMetadata::getKey() const {
  return getKeyId3(wavFile->tag());
}

bool WavID3FileMetadata::setTitle(const QString& tit) {
  wavFile->tag()->setTitle(
    TagLib::String(tit.toUtf8().constData(), TagLib::String::UTF8)
  );
  wavFile->save();
  return true;
}

bool WavID3FileMetadata::setArtist(const QString& art) {
  wavFile->tag()->setArtist(
    TagLib::String(art.toUtf8().constData(), TagLib::String::UTF8)
  );
  wavFile->save();
  return true;
}

bool WavID3FileMetadata::setAlbum(const QString& alb) {
  wavFile->tag()->setAlbum(
    TagLib::String(alb.toUtf8().constData(), TagLib::String::UTF8)
  );
  wavFile->save();
  return true;
}

bool WavID3FileMetadata::setComment(const QString& cmt) {
  genericFile->tag()->setComment(
    TagLib::String(cmt.toUtf8().constData(), TagLib::String::UTF8)
  );
  genericFile->save();
  return true;
}

bool WavID3FileMetadata::setGrouping(const QString& grp) {
  setGroupingId3(wavFile->tag(), grp);
  wavFile->save();
  return true;
}

bool WavID3FileMetadata::setKey(const QString& key) {
  setKeyId3(wavFile->tag(), key);
  wavFile->save();
  return true;
}

// =================================== MP4 =====================================

QString Mp4FileMetadata::getGrouping() const {
  if (!mp4File->tag()->itemListMap().contains(keyMp4TagGrouping))
    return emptyString;
  TagLib::MP4::Item m = mp4File->tag()->itemListMap()[keyMp4TagGrouping];
  TagLib::String out = m.toStringList().front();
  return QString::fromUtf8((out.toCString(true)));
}

QString Mp4FileMetadata::getKey() const {
  if (!mp4File->tag()->itemListMap().contains(keyMp4TagKey))
    return emptyString;
  TagLib::MP4::Item m = mp4File->tag()->itemListMap()[keyMp4TagKey];
  TagLib::String out = m.toStringList().front();
  return QString::fromUtf8((out.toCString(true)));
}

bool Mp4FileMetadata::setGrouping(const QString& grp) {
  TagLib::StringList sl(TagLib::String(grp.toUtf8().constData(), TagLib::String::UTF8));
  mp4File->tag()->itemListMap().insert(keyMp4TagGrouping, sl);
  mp4File->save();
  return true;
}

bool Mp4FileMetadata::setKey(const QString& key) {
  TagLib::StringList sl(TagLib::String(key.toUtf8().constData(), TagLib::String::UTF8));
  mp4File->tag()->itemListMap().insert(keyMp4TagKey, sl);
  mp4File->save();
  return true;
}

// =================================== ASF =====================================

QString AsfFileMetadata::getGrouping() const {
  if (!asfFile->tag()->attributeListMap().contains(keyAsfTagGrouping))
    return emptyString;
  TagLib::ASF::AttributeList l = asfFile->tag()->attributeListMap()[keyAsfTagGrouping];
  TagLib::String out = l.front().toString();
  return QString::fromUtf8((out.toCString(true)));
}

QString AsfFileMetadata::getKey() const {
  if (!asfFile->tag()->attributeListMap().contains(keyAsfTagKey))
    return emptyString;
  TagLib::ASF::AttributeList l = asfFile->tag()->attributeListMap()[keyAsfTagKey];
  TagLib::String out = l.front().toString();
  return QString::fromUtf8((out.toCString(true)));
}

bool AsfFileMetadata::setGrouping(const QString& grp) {
  asfFile->tag()->setAttribute(
    keyAsfTagGrouping,
    TagLib::String(grp.toUtf8().constData(), TagLib::String::UTF8)
  );
  asfFile->save();
  return true;
}

bool AsfFileMetadata::setKey(const QString& key) {
  asfFile->tag()->setAttribute(
    keyAsfTagKey,
    TagLib::String(key.toUtf8().constData(), TagLib::String::UTF8)
  );
  asfFile->save();
  return true;
}
