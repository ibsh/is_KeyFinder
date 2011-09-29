/*************************************************************************

	Copyright 2011 Ibrahim Sha'ath

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

TagLibMetadata::TagLibMetadata(const QString& filePath){

  QString fileExt = filePath.right(filePath.length() - filePath.lastIndexOf(".") - 1).toLower();
  char* filePathCh = filePath.toAscii().data();

  if(fileExt == "mp3"){
    f = new TagLib::MPEG::File(filePathCh);
    return;
  }

  #ifdef TAGLIB_WITH_MP4
    if(fileExt == "m4a" || fileExt == "m4b" || fileExt == "m4p" || fileExt == "mp4" || fileExt == "3g2"){
      f = new TagLib::MP4::File(filePathCh);
      return;
    }
  #endif

  #ifdef TAGLIB_WITH_ASF
    if(fileExt == "wma" || fileExt == "asf"){
      f = new TagLib::ASF::File(filePathCh);
      return;
    }
  #endif

  if(fileExt == "aif" || fileExt == "aiff"){
    f = new TagLib::RIFF::AIFF::File(filePathCh);
    return;
  }

  if(fileExt == "wav"){
    f = new TagLib::RIFF::WAV::File(filePathCh);
    return;
  }

  if(fileExt == "ogg"){
    f = new TagLib::Ogg::Vorbis::File(filePathCh);
    return;
  }

  if(fileExt == "oga"){
    f = new TagLib::Ogg::FLAC::File(filePathCh);
    if (f->isValid()){
      return;
    }
    delete f;
    f = new TagLib::Ogg::Vorbis::File(filePathCh);
    return;
  }

  if(fileExt == "flac"){
    f = new TagLib::FLAC::File(filePathCh);
    return;
  }

  if(fileExt == "ape"){
    f = new TagLib::APE::File(filePathCh);
    return;
  }

  if(fileExt == "mpc"){
    f = new TagLib::MPC::File(filePathCh);
    return;
  }

  if(fileExt == "wv"){
    f = new TagLib::WavPack::File(filePathCh);
    return;
  }

  if(fileExt == "spx"){
    f = new TagLib::Ogg::Speex::File(filePathCh);
    return;
  }

  if(fileExt == "tta"){
    f = new TagLib::TrueAudio::File(filePathCh);
    return;
  }

  f = NULL;
  qDebug("TagLib returned NULL File for %s",filePathCh);
  return;

}

TagLibMetadata::~TagLibMetadata(){
  if(f != NULL)
    delete f;
}

QString TagLibMetadata::getTitle() const{
  if(f == NULL || !f->isValid())
		return "";
  TagLib::String out = f->tag()->title();
  return QString::fromUtf8(out.toCString(true));
}

QString TagLibMetadata::getArtist() const{
  if(f == NULL || !f->isValid())
		return "";
  TagLib::String out = f->tag()->artist();
  return QString::fromUtf8(out.toCString(true));
}

QString TagLibMetadata::getComment() const{
  if(f == NULL || !f->isValid())
    return "";
  TagLib::String out = f->tag()->comment();
  return QString::fromUtf8(out.toCString(true));
}

QString TagLibMetadata::getGrouping() const{

  if(f == NULL || !f->isValid())
		return "";

	TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
	if(fileTestMpeg != NULL){
		TagLib::ID3v2::Tag* tagTestId3v2 = fileTestMpeg->ID3v2Tag();
		if(tagTestId3v2 != NULL){
			TagLib::ID3v2::FrameList l = tagTestId3v2->frameListMap()["TIT1"];
			if(!l.isEmpty()){
				TagLib::String out = l.front()->toString();
				return QString::fromUtf8((out.toCString()));
			}
			return "";
		}
		TagLib::ID3v1::Tag* tagTestId3v1 = fileTestMpeg->ID3v1Tag();
		if(tagTestId3v1 != NULL){
      qDebug("ID3v1 does not support the Grouping tag (%s)",f->name());
			return "N/A";
		}
	}

	TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
	if(fileTestAiff != NULL){
    TagLib::ID3v2::Tag* tagTestId3v2 = fileTestAiff->tag();
		if(tagTestId3v2 != NULL){
			TagLib::ID3v2::FrameList l = tagTestId3v2->frameListMap()["TIT1"];
			if(!l.isEmpty()){
				TagLib::String out = l.front()->toString();
				return QString::fromUtf8((out.toCString()));
			}
			return "";
		}
	}

	TagLib::RIFF::WAV::File* fileTestWav = dynamic_cast<TagLib::RIFF::WAV::File*>(f);
	if(fileTestWav != NULL){
		TagLib::ID3v2::Tag* tagTestId3v2 = fileTestWav->tag();
		if(tagTestId3v2 != NULL){
			TagLib::ID3v2::FrameList l = tagTestId3v2->frameListMap()["TIT1"];
			if(!l.isEmpty()){
				TagLib::String out = l.front()->toString();
				return QString::fromUtf8((out.toCString()));
			}
			return "";
		}
	}

  TagLib::MP4::Tag* tagTestMp4 = dynamic_cast<TagLib::MP4::Tag*>(f->tag());
	if(tagTestMp4 != NULL){
		TagLib::MP4::Item m = tagTestMp4->itemListMap()["\251grp"];
		if(m.isValid()){
			TagLib::String out = m.toStringList().front();
			return QString::fromUtf8((out.toCString()));
		}
		return "";
	}

  TagLib::ASF::Tag* tagTestAsf = dynamic_cast<TagLib::ASF::Tag*>(f->tag());
	if(tagTestAsf != NULL){
    TagLib::ASF::AttributeList l = tagTestAsf->attributeListMap()["WM/ContentGroupDescription"];
		if(!l.isEmpty()){
			TagLib::String out = l.front().toString();
			return QString::fromUtf8((out.toCString()));
		}
		return "";
	}

  TagLib::APE::Tag* tagTestApe = dynamic_cast<TagLib::APE::Tag*>(f->tag());
	if(tagTestApe != NULL){
		TagLib::APE::Item m = tagTestApe->itemListMap()["Grouping"];
		if(!m.isEmpty()){
			TagLib::String out = m.toStringList().front();
			return QString::fromUtf8((out.toCString()));
		}
		return "";
	}

  qDebug("Tag read failed all tests on %s",f->name());
	return "N/A";
}

int TagLibMetadata::setComment(const QString& cmt){
  if(f == NULL || !f->isValid()){
    qDebug("Cannot set comment tag on invalid file object");
    return 1;
  }
  f->tag()->setComment(TagLib::String(cmt.toAscii().data()));
  f->save();
  return 0;
}

int TagLibMetadata::setGrouping(const QString& grp){

  if(f == NULL || !f->isValid()){
		qDebug("Cannot set grouping tag on invalid file object");
    return 1;
	}

	TagLib::MPEG::File* fileTestMpeg = dynamic_cast<TagLib::MPEG::File*>(f);
	if(fileTestMpeg != NULL){
		TagLib::ID3v2::Tag* tagTestId3v2 = fileTestMpeg->ID3v2Tag();
		if(tagTestId3v2 != NULL){
			TagLib::ID3v2::Frame* frm = new TagLib::ID3v2::TextIdentificationFrame("TIT1");
			frm->setText(TagLib::String(grp.toAscii().data()));
			tagTestId3v2->removeFrames("TIT1");
			tagTestId3v2->addFrame(frm);
      f->save();
      return 0;
		}else{
			TagLib::ID3v1::Tag* tagTestId3v1 = fileTestMpeg->ID3v1Tag();
			if(tagTestId3v1 != NULL){
        qDebug("ID3v1 does not support the Grouping tag (%s)",f->name());
        return 1;
			}
		}
	}

	TagLib::RIFF::AIFF::File* fileTestAiff = dynamic_cast<TagLib::RIFF::AIFF::File*>(f);
	if(fileTestAiff != NULL){
    TagLib::ID3v2::Tag* tagTestId3v2 = fileTestAiff->tag();
		if(tagTestId3v2 != NULL){
			TagLib::ID3v2::Frame* frm = new TagLib::ID3v2::TextIdentificationFrame("TIT1");
			frm->setText(TagLib::String(grp.toAscii().data()));
			tagTestId3v2->removeFrames("TIT1");
			tagTestId3v2->addFrame(frm);
      f->save();
      return 0;
		}
	}

	TagLib::RIFF::WAV::File* fileTestWav = dynamic_cast<TagLib::RIFF::WAV::File*>(f);
	if(fileTestWav != NULL){
		TagLib::ID3v2::Tag* tagTestId3v2 = fileTestWav->tag();
		if(tagTestId3v2 != NULL){
			TagLib::ID3v2::Frame* frm = new TagLib::ID3v2::TextIdentificationFrame("TIT1");
			frm->setText(TagLib::String(grp.toAscii().data()));
			tagTestId3v2->removeFrames("TIT1");
			tagTestId3v2->addFrame(frm);
      f->save();
      return 0;
		}
	}

  TagLib::MP4::Tag* tagTestMp4 = dynamic_cast<TagLib::MP4::Tag*>(f->tag());
	if(tagTestMp4 != NULL){
		TagLib::StringList sl(TagLib::String(grp.toAscii().data()));
		tagTestMp4->itemListMap()["\251grp"] = sl;
		tagTestMp4->save();
    f->save();
    return 0;
	}

  TagLib::ASF::Tag* tagTestAsf = dynamic_cast<TagLib::ASF::Tag*>(f->tag());
	if(tagTestAsf != NULL){
    tagTestAsf->setAttribute("WM/ContentGroupDescription",TagLib::String(grp.toAscii().data()));
    f->save();
    return 0;
	}

  TagLib::APE::Tag* tagTestApe = dynamic_cast<TagLib::APE::Tag*>(f->tag());
	if(tagTestApe != NULL){
		tagTestApe->addValue("GROUPING",TagLib::String(grp.toAscii().data()));
    f->save();
    return 0;
	}

  qDebug("Tag write failed all tests on %s",f->name());
  return 1;
}
