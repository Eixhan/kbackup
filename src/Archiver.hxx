//**************************************************************************
//   Copyright 2006 - 2017 Martin Koller, kollix@aon.at
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, version 2 of the License
//
//**************************************************************************

#ifndef _ARCHIVER_H_
#define _ARCHIVER_H_

// the class which does the archiving

#include <QObject>
#include <QPointer>
#include <QSet>
#include <QElapsedTimer>
#include <QTime>
#include <QDateTime>
#include <QStringList>
#include <QList>
#include <QRegExp>

#include <QUrl>
#include <kio/copyjob.h>
#include <kio/udsentry.h>
#include <KCompressionDevice>

class KTar;
class QDir;
class QFileInfo;
class QFile;


class Archiver : public QObject
{
  Q_OBJECT

  public:
    explicit Archiver(QWidget *parent);

    static Archiver *instance;

    // always call after you have already set maxSliceMBs, as the sliceCapacity
    // might be limited with it
    void setTarget(const QUrl &target);
    const QUrl &getTarget() const { return targetURL; }

    enum { UNLIMITED = 0 };
    void setMaxSliceMBs(int mbs);
    int getMaxSliceMBs() const { return maxSliceMBs; }

    void setFilePrefix(const QString &prefix);
    const QString &getFilePrefix() const { return filePrefix; }

    void setMediaNeedsChange(bool b) { mediaNeedsChange = b; }
    bool getMediaNeedsChange() const { return mediaNeedsChange; }

    void setCompressFiles(bool b);
    bool getCompressFiles() const { return !ext.isEmpty(); }

    // number of backups to keep before older ones will be deleted (UNLIMITED or 1..n)
    void setKeptBackups(int num);
    int getKeptBackups() const { return numKeptBackups; }

    // define a filename filter in wildcard format each separated with a space
    // e.g. "*.png *.ogg"
    void setFilter(const QString &filter);
    QString getFilter() const;

    // define a list of path wildcards to filter complete dirs, separated by newline
    void setDirFilter(const QString &filter);
    QString getDirFilter() const;

    // interval for a full backup instead differential backup; when 1 given == full backup
    void setFullBackupInterval(int days);
    int getFullBackupInterval() const { return fullBackupInterval; }

    const QDateTime &getLastFullBackup() const { return lastFullBackup; }
    const QDateTime &getLastBackup() const { return lastBackup; }

    // print every single file/dir in non-interactive mode
    void setVerbose(bool b) { verbose = b; }

    // loads the profile into the Archiver and returns includes/excludes lists
    // return true if loaded, false on file open error
    bool loadProfile(const QString &fileName, QStringList &includes, QStringList &excludes, QString &error);
    void setLoadedProfile(const QString &fileName) { loadedProfile = fileName; }

    bool saveProfile(const QString &fileName, const QStringList &includes, const QStringList &excludes, QString &error);

    // return true if the backup completed successfully, else false
    bool createArchive(const QStringList &includes, const QStringList &excludes);

    KIO::filesize_t getTotalBytes() const { return totalBytes; }
    int getTotalFiles() const { return totalFiles; }

    bool isInProgress() const { return runs; }

    static bool getDiskFree(const QString &path, KIO::filesize_t &capacityBytes, KIO::filesize_t &freeBytes);

    // TODO: put probably in some global settings object
    static QString sliceScript;

  public slots:
    void cancel();  // cancel a running creation
    void setForceFullBackup(bool force = true);

  signals:
    void inProgress(bool runs) const;
    void logging(const QString &) const;
    void warning(const QString &) const;
    void targetCapacity(KIO::filesize_t bytes) const;
    void sliceProgress(int percent) const;
    void fileProgress(int percent) const;
    void newSlice(int) const;
    void totalFilesChanged(int) const;
    void totalBytesChanged(KIO::filesize_t) const;
    void elapsedChanged(const QTime &) const;
    void backupTypeChanged(bool incremental) const;

  private slots:
    void slotResult(KJob *);
    void slotListResult(KIO::Job *, const KIO::UDSEntryList &);
    void receivedOutput();
    void loggingSlot(const QString &message); // for non-interactive output
    void warningSlot(const QString &message); // for non-interactive output
    void updateElapsed();

  private:
    void calculateCapacity();  // also emits signals
    void addDirFiles(QDir &dir);
    void addFile(const QFileInfo &info);

    enum AddFileStatus { Error, Added, Skipped };
    AddFileStatus addLocalFile(const QFileInfo &info);

    bool compressFile(const QString &origName, QFile &comprFile);

    void finishSlice();
    bool getNextSlice();

    void runScript(const QString &mode);
    void setIncrementalBackup(bool inc);

    // returns true if the next backup will be an incremental one, false for a full backup
    bool isIncrementalBackup() const { return !forceFullBackup && incrementalBackup; }

    // return true if given fileName matches any of the defined filters
    bool fileIsFiltered(const QString &fileName) const;

    void emitArchiveError() const;

    static bool UDSlessThan(const KIO::UDSEntry &left, const KIO::UDSEntry &right);

  private:
    QSet<QString> excludeFiles;
    QSet<QString> excludeDirs;

    QString archiveName;
    QString filePrefix;  // default = "backup"
    QStringList sliceList;
    QString loadedProfile;

    KTar *archive;
    KIO::filesize_t totalBytes;
    int totalFiles;
    int filteredFiles;  // filter or time filter (incremental backup)
    QElapsedTimer elapsed;

    QList<QRegExp> filters;
    QList<QRegExp> dirFilters;

    QUrl targetURL;
    QString baseName;
    int sliceNum;
    int maxSliceMBs;
    bool mediaNeedsChange;
    bool compressFiles;

    int numKeptBackups;
    KIO::UDSEntryList targetDirList;

    QDateTime lastFullBackup;
    QDateTime lastBackup;
    int fullBackupInterval;
    bool incrementalBackup;
    bool forceFullBackup;

    KIO::filesize_t sliceBytes;
    KIO::filesize_t sliceCapacity;

    QString ext;
    KCompressionDevice::CompressionType compressionType;

    bool interactive;
    bool cancelled;
    bool runs;
    bool skippedFiles;  // did we skip files during backup ?
    bool verbose;

    QPointer<KIO::CopyJob> job;
    int jobResult;
};

#endif
