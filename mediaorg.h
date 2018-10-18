#ifndef MEDIAORG_H
#define MEDIAORG_H

#include "includer.h"

namespace mediaOrg {
struct mediaObjectData {
    int id;
    QFileInfo file;
    mediaObjectData(int dbID, QFileInfo fileInfo) : id(dbID), file(fileInfo) { }
    mediaObjectData operator= (mediaObjectData const & d) {
        file = d.file;
        id = d.id;
        mediaObjectData newInfo(id, file);
        return(newInfo);
    }
};


}

#endif // MEDIAORG_H
