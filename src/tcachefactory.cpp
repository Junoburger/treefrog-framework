#include "tcachefactory.h"
#include "tcachesqlitestore.h"
#include "tsystemglobal.h"
#include <TAppSettings>
#include <QDir>

static QString SINGLEFILE_CACHE_KEY;


static void loadCacheKeys()
{
    static bool done = []() {
        SINGLEFILE_CACHE_KEY = TCacheSQLiteStore().key().toLower();
        return true;
    }();
    Q_UNUSED(done);
}


QStringList TCacheFactory::keys()
{
    loadCacheKeys();

    QStringList ret;
    ret << SINGLEFILE_CACHE_KEY;
    return ret;
}


TCacheStore *TCacheFactory::create(const QString &key)
{
    TCacheStore *ptr = nullptr;
    loadCacheKeys();

    QString k = key.toLower();
    if (k == SINGLEFILE_CACHE_KEY) {
        static const int FileSizeThreshold = TAppSettings::instance()->value(Tf::CacheSingleFileFileSizeThreshold).toInt();
        static const QString filepath = [] {
            QString path = TAppSettings::instance()->value(Tf::CacheSingleFileFilePath).toString().trimmed();
            if (!path.isEmpty() && QDir::isRelativePath(path)) {
                path = Tf::app()->webRootPath() + path;
            }
            return path;
        }();

        ptr = new TCacheSQLiteStore(filepath, FileSizeThreshold);

    } else {
        tSystemError("Not found cache store: %s", qPrintable(key));
    }

    return ptr;
}


void TCacheFactory::destroy(const QString &key, TCacheStore *store)
{
    loadCacheKeys();

    QString k = key.toLower();
    if (k == SINGLEFILE_CACHE_KEY) {
        delete store;
    }
}
