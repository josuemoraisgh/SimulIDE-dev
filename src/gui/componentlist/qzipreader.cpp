/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qglobal.h>

#include "qzipreader.h"
#include <qdatetime.h>
#include <qplatformdefs.h>
#include <qendian.h>
#include <qdebug.h>
#include <qdir.h>

#include <zlib.h>

#if defined(Q_OS_WIN)
#undef S_IFREG
#define S_IFREG 0100000
#  ifndef S_ISDIR
#    define S_ISDIR(x) ((x) & 0040000) > 0
#  endif
#  ifndef S_ISREG
#    define S_ISREG(x) ((x) & 0170000) == S_IFREG
#  endif
#  define S_IFLNK 020000
#  define S_ISLNK(x) ((x) & S_IFLNK) > 0
#  ifndef S_IRUSR
#    define S_IRUSR 0400
#  endif
#  ifndef S_IWUSR
#    define S_IWUSR 0200
#  endif
#  ifndef S_IXUSR
#    define S_IXUSR 0100
#  endif
#  define S_IRGRP 0040
#  define S_IWGRP 0020
#  define S_IXGRP 0010
#  define S_IROTH 0004
#  define S_IWOTH 0002
#  define S_IXOTH 0001
#endif

#if 0
#define ZDEBUG qDebug
#else
#define ZDEBUG if( 0) qDebug
#endif

QT_BEGIN_NAMESPACE

static inline uint readUInt(const uchar *data)
{
    return (data[0]) + (data[1]<<8) + (data[2]<<16) + (data[3]<<24);
}

static inline ushort readUShort(const uchar *data)
{
    return (data[0]) + (data[1]<<8);
}

static inline void writeUInt(uchar *data, uint i)
{
    data[0] = i & 0xff;
    data[1] = (i>>8) & 0xff;
    data[2] = (i>>16) & 0xff;
    data[3] = (i>>24) & 0xff;
}

static inline void writeUShort(uchar *data, ushort i)
{
    data[0] = i & 0xff;
    data[1] = (i>>8) & 0xff;
}

static inline void copyUInt(uchar *dest, const uchar *src)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    dest[3] = src[3];
}

static inline void copyUShort(uchar *dest, const uchar *src)
{
    dest[0] = src[0];
    dest[1] = src[1];
}

static int inflate(Bytef *dest, ulong *destLen, const Bytef *source, ulong sourceLen)
{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
    if( (uLong)stream.avail_in != sourceLen)
        return Z_BUF_ERROR;

    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if( (uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;

    err = inflateInit2(&stream, -MAX_WBITS);
    if( err != Z_OK) return err;

    err = inflate(&stream, Z_FINISH);
    if( err != Z_STREAM_END) {
        inflateEnd(&stream);
        if( err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
            return Z_DATA_ERROR;
        return err;
    }
    *destLen = stream.total_out;

    err = inflateEnd(&stream);
    return err;
}

static QFile::Permissions modeToPermissions(quint32 mode)
{
    QFile::Permissions ret;
    if( mode & S_IRUSR) ret |= QFile::ReadOwner;
    if( mode & S_IWUSR) ret |= QFile::WriteOwner;
    if( mode & S_IXUSR) ret |= QFile::ExeOwner;
    if( mode & S_IRUSR) ret |= QFile::ReadUser;
    if( mode & S_IWUSR) ret |= QFile::WriteUser;
    if( mode & S_IXUSR) ret |= QFile::ExeUser;
    if( mode & S_IRGRP) ret |= QFile::ReadGroup;
    if( mode & S_IWGRP) ret |= QFile::WriteGroup;
    if( mode & S_IXGRP) ret |= QFile::ExeGroup;
    if( mode & S_IROTH) ret |= QFile::ReadOther;
    if( mode & S_IWOTH) ret |= QFile::WriteOther;
    if( mode & S_IXOTH) ret |= QFile::ExeOther;
    return ret;
}

struct LocalFileHeader
{
    uchar signature[4]; //  0x04034b50
    uchar version_needed[2];
    uchar general_purpose_bits[2];
    uchar compression_method[2];
    uchar last_mod_file[4];
    uchar crc_32[4];
    uchar compressed_size[4];
    uchar uncompressed_size[4];
    uchar file_name_length[2];
    uchar extra_field_length[2];
};

struct DataDescriptor
{
    uchar crc_32[4];
    uchar compressed_size[4];
    uchar uncompressed_size[4];
};

struct CentralFileHeader
{
    uchar signature[4]; // 0x02014b50
    uchar version_made[2];
    uchar version_needed[2];
    uchar general_purpose_bits[2];
    uchar compression_method[2];
    uchar last_mod_file[4];
    uchar crc_32[4];
    uchar compressed_size[4];
    uchar uncompressed_size[4];
    uchar file_name_length[2];
    uchar extra_field_length[2];
    uchar file_comment_length[2];
    uchar disk_start[2];
    uchar internal_file_attributes[2];
    uchar external_file_attributes[4];
    uchar offset_local_header[4];
    LocalFileHeader toLocalHeader() const;
};

struct EndOfDirectory
{
    uchar signature[4]; // 0x06054b50
    uchar this_disk[2];
    uchar start_of_directory_disk[2];
    uchar num_dir_entries_this_disk[2];
    uchar num_dir_entries[2];
    uchar directory_size[4];
    uchar dir_start_offset[4];
    uchar comment_length[2];
};

struct FileHeader
{
    CentralFileHeader h;
    QByteArray file_name;
    QByteArray extra_field;
    QByteArray file_comment;
};

qZipReader::FileInfo::FileInfo()
    : isDir(false), isFile(true), isSymLink(false), crc32(0), size(0)
{
}

qZipReader::FileInfo::~FileInfo()
{
}

qZipReader::FileInfo::FileInfo(const FileInfo &other)
{
    operator=(other);
}

qZipReader::FileInfo& qZipReader::FileInfo::operator=(const FileInfo &other)
{
    filePath = other.filePath;
    isDir = other.isDir;
    isFile = other.isFile;
    isSymLink = other.isSymLink;
    permissions = other.permissions;
    crc32 = other.crc32;
    size = other.size;
    return *this;
}

class qZipPrivate
{
    public:
        qZipPrivate(QIODevice *device, bool ownDev)
            : device(device), ownDevice(ownDev), dirtyFileTree(true), start_of_directory(0)
        {}

        ~qZipPrivate() {if( ownDevice) delete device;}

        void fillFileInfo(int index, qZipReader::FileInfo &fileInfo) const;

        QIODevice *device;
        bool ownDevice;
        bool dirtyFileTree;
        QList<FileHeader> fileHeaders;
        QByteArray comment;
        uint start_of_directory;
};

void qZipPrivate::fillFileInfo(int index, qZipReader::FileInfo &fileInfo) const
{
    FileHeader header = fileHeaders.at(index);
    fileInfo.filePath = QString::fromLocal8Bit(header.file_name);
    const quint32 mode = (qFromLittleEndian<quint32>(&header.h.external_file_attributes[0]) >> 16) & 0xFFFF;
    fileInfo.isDir = S_ISDIR(mode);
    fileInfo.isFile = S_ISREG(mode);
    fileInfo.isSymLink = S_ISLNK(mode);
    fileInfo.permissions = modeToPermissions(mode);
    fileInfo.crc32 = readUInt(header.h.crc_32);
    fileInfo.size = readUInt(header.h.uncompressed_size);
}

class qZipReaderPrivate : public qZipPrivate
{
    public:
        qZipReaderPrivate(QIODevice *device, bool ownDev)
            : qZipPrivate(device, ownDev), status(qZipReader::NoError)
        {}

        void scanFiles();

        qZipReader::Status status;
};

LocalFileHeader CentralFileHeader::toLocalHeader() const
{
    LocalFileHeader h;
    writeUInt(h.signature, 0x04034b50);
    copyUShort(h.version_needed, version_needed);
    copyUShort(h.general_purpose_bits, general_purpose_bits);
    copyUShort(h.compression_method, compression_method);
    copyUInt(h.last_mod_file, last_mod_file);
    copyUInt(h.crc_32, crc_32);
    copyUInt(h.compressed_size, compressed_size);
    copyUInt(h.uncompressed_size, uncompressed_size);
    copyUShort(h.file_name_length, file_name_length);
    copyUShort(h.extra_field_length, extra_field_length);
    return h;
}

void qZipReaderPrivate::scanFiles()
{
    if( !dirtyFileTree ) return;

    if( !( device->isOpen() || device->open(QIODevice::ReadOnly))) {
        status = qZipReader::FileOpenError;
        return;
    }

    if( (device->openMode() & QIODevice::ReadOnly) == 0) { // only read the index from readable files.
        status = qZipReader::FileReadError;
        return;
    }

    dirtyFileTree = false;
    uchar tmp[4];
    device->read((char *)tmp, 4);

    if( readUInt(tmp) != 0x04034b50) { qWarning() << "qZip: not a zip file!"; return; }

    // find EndOfDirectory header
    int i = 0;
    int start_of_directory = -1;
    int num_dir_entries = 0;
    EndOfDirectory eod;
    while (start_of_directory == -1) {
        int pos = device->size() - sizeof(EndOfDirectory) - i;
        if( pos < 0 || i > 65535 ) { qWarning() << "qZip: EndOfDirectory not found"; return; }

        device->seek(pos);
        device->read((char *)&eod, sizeof(EndOfDirectory));
        if( readUInt(eod.signature) == 0x06054b50) break;
        ++i;
    }

    // have the eod
    start_of_directory = readUInt(eod.dir_start_offset);
    num_dir_entries = readUShort(eod.num_dir_entries);
    ZDEBUG("start_of_directory at %d, num_dir_entries=%d", start_of_directory, num_dir_entries);
    int comment_length = readUShort(eod.comment_length);
    if( comment_length != i) qWarning() << "qZip: failed to parse zip file.";
    comment = device->read( qMin(comment_length, i) );

    device->seek(start_of_directory);
    for (i = 0; i < num_dir_entries; ++i) {
        FileHeader header;
        int read = device->read((char *) &header.h, sizeof(CentralFileHeader));
        if( read < (int)sizeof(CentralFileHeader)) {
            qWarning() << "qZip: Failed to read complete header, index may be incomplete";
            break;
        }
        if( readUInt(header.h.signature) != 0x02014b50) {
            qWarning() << "qZip: invalid header signature, index may be incomplete";
            break;
        }

        int l = readUShort(header.h.file_name_length);
        header.file_name = device->read(l);
        if( header.file_name.length() != l) {
            qWarning() << "qZip: Failed to read filename from zip index, index may be incomplete";
            break;
        }
        l = readUShort(header.h.extra_field_length);
        header.extra_field = device->read(l);
        if( header.extra_field.length() != l) {
            qWarning() << "qZip: Failed to read extra field in zip file, skipping file, index may be incomplete";
            break;
        }
        l = readUShort(header.h.file_comment_length);
        header.file_comment = device->read(l);
        if( header.file_comment.length() != l) {
            qWarning() << "qZip: Failed to read read file comment, index may be incomplete";
            break;
        }

        ZDEBUG("found file '%s'", header.file_name.data());
        fileHeaders.append(header);
    }
}

qZipReader::qZipReader( const QString &archive, QIODevice::OpenMode mode )
{
    QScopedPointer<QFile> f( new QFile(archive) );
    f->open(mode);
    qZipReader::Status status;
    if( f->error() == QFile::NoError) status = NoError;
    else {
        if      (f->error() == QFile::ReadError)        status = FileReadError;
        else if( f->error() == QFile::OpenError)        status = FileOpenError;
        else if( f->error() == QFile::PermissionsError) status = FilePermissionsError;
        else                                            status = FileError;
    }
    d = new qZipReaderPrivate( f.data(), /*ownDevice=*/true );
    f.take();
    d->status = status;
}

/*!
    Create a new zip archive that operates on the archive found in \a device.
    You have to open the device previous to calling the constructor and only a
    device that is readable will be scanned for zip filecontent.
 */
qZipReader::qZipReader( QIODevice* device )
          : d( new qZipReaderPrivate(device, /*ownDevice=*/false) )
{
    Q_ASSERT(device);
}
qZipReader::~qZipReader()
{
    close();
    delete d;
}

bool qZipReader::isReadable() const
{
    return d->device->isReadable();
}

bool qZipReader::exists() const
{
    QFile *f = qobject_cast<QFile*> (d->device);
    if( f == 0) return true;
    return f->exists();
}

QList<qZipReader::FileInfo> qZipReader::fileInfoList() const
{
    d->scanFiles();
    QList<qZipReader::FileInfo> files;
    for (int i = 0; i < d->fileHeaders.size(); ++i) {
        qZipReader::FileInfo fi;
        d->fillFileInfo(i, fi);
        files.append(fi);
    }
    return files;
}

int qZipReader::count() const
{
    d->scanFiles();
    return d->fileHeaders.count();
}

qZipReader::FileInfo qZipReader::entryInfoAt( int index ) const
{
    d->scanFiles();
    qZipReader::FileInfo fi;
    d->fillFileInfo(index, fi);
    return fi;
}

QByteArray qZipReader::fileData( const QString &fileName ) const
{
    d->scanFiles();
    int i;
    for (i = 0; i < d->fileHeaders.size(); ++i) {
        if( QString::fromLocal8Bit( d->fileHeaders.at(i).file_name) == fileName )
            break;
    }
    if( i == d->fileHeaders.size() ) return QByteArray();

    FileHeader header = d->fileHeaders.at(i);

    int compressed_size = readUInt(header.h.compressed_size);
    int uncompressed_size = readUInt(header.h.uncompressed_size);
    int start = readUInt(header.h.offset_local_header);
    //qDebug("uncompressing file %d: local header at %d", i, start);

    d->device->seek(start);
    LocalFileHeader lh;
    d->device->read((char *)&lh, sizeof(LocalFileHeader));
    uint skip = readUShort(lh.file_name_length) + readUShort(lh.extra_field_length);
    d->device->seek(d->device->pos() + skip);

    int compression_method = readUShort(lh.compression_method);
    //qDebug("file=%s: compressed_size=%d, uncompressed_size=%d", fileName.toLocal8Bit().data(), compressed_size, uncompressed_size);

    //qDebug("file at %lld", d->device->pos());
    QByteArray compressed = d->device->read( compressed_size );
    if( compression_method == 0) {
        // no compression
        compressed.truncate(uncompressed_size);
        return compressed;
    } else if( compression_method == 8) {
        // Deflate
        //qDebug("compressed=%d", compressed.size());
        compressed.truncate(compressed_size);
        QByteArray baunzip;
        ulong len = qMax(uncompressed_size,  1);
        int res;
        do {
            baunzip.resize(len);
            res = inflate((uchar*)baunzip.data(), &len,
                          (uchar*)compressed.constData(), compressed_size);

            switch (res) {
            case Z_OK:
                if( (int)len != baunzip.size()) baunzip.resize(len);
                break;
            case Z_MEM_ERROR:
                qWarning("qZip: Z_MEM_ERROR: Not enough memory");
                break;
            case Z_BUF_ERROR:
                len *= 2;
                break;
            case Z_DATA_ERROR:
                qWarning("qZip: Z_DATA_ERROR: Input data is corrupted");
                break;
            }
        } while (res == Z_BUF_ERROR);
        return baunzip;
    }
    qWarning() << "qZip: Unknown compression method";
    return QByteArray();
}

bool qZipReader::extractAll( const QString &destinationDir ) const
{
    QDir baseDir( destinationDir ) ;

    QList<FileInfo> allFiles = fileInfoList(); // create directories first
    foreach( FileInfo fi, allFiles ) {
        const QString absPath = destinationDir + QDir::separator() + fi.filePath;
        if( fi.isDir) {
            if( !baseDir.mkpath( fi.filePath )) return false;
            if( !QFile::setPermissions( absPath, fi.permissions )) return false;
        }
    }

    foreach( FileInfo fi, allFiles )
    {
        const QString absPath = destinationDir + QDir::separator() + fi.filePath;
        if( fi.isFile ) {
            QFile f( absPath );
            if( f.exists() ) QFile::remove( absPath );
            if( !f.open( QIODevice::WriteOnly )) return false;
            f.write( fileData(fi.filePath) );
            f.setPermissions( fi.permissions );
            f.close();
        }
    }
    return true;
}

qZipReader::Status qZipReader::status() const
{
    return d->status;
}

void qZipReader::close()
{
    d->device->close();
}

QT_END_NAMESPACE
