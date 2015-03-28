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

#include "asyncmetadatareadprocess.h"

MetadataReadResult metadataReadProcess(const AsyncFileObject& object) {

  MetadataReadResult result;
  result.batchRow = object.batchRow;

  AVFileMetadataFactory factory;
  AVFileMetadata* md = factory.createAVFileMetadata(object.filePath);

  for (unsigned int i = 0; i < METADATA_TAG_T_COUNT; i++) {
    result.tags.push_back(md->getByTagEnum((metadata_tag_t) i));
  }

  delete md;

  return result;
}
