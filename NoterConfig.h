#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <QDir>
#include <QFile>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QStringView>
#include <unordered_map>

/*******************************************************************************
 * @brief NoterConfig is designed to read, parse and process configuration file
 *
 * NoterConfig perhaps is not valid, so caller must ensure it is valid before
 * using it. NoterConfig is not valid only when configuration file is erroneous.
 * If configuration is empty, NoterConfig is valid.
 *
 * If NoterConfig is not valid or configuration is empty, all getter return empty
 * value.
 ******************************************************************************/
class NoterConfig
{
  public:
    NoterConfig();
    QString getNotebookPath(const QString& notebook) const;
    QStringList getNotebooks() const;
    QString getLastOpenedNotebook() const;
    QJsonObject::iterator setObject(const QString& key,
                                    const QJsonValue& value);
    QJsonObject::iterator addNotebook(const QString& notebook,
                                      const QString& path);
    QJsonObject::iterator setLastOpenedNotebook(const QString &lastOpenedNotebook);
    void removeNotebook(const QString& notebook);
    void removeLastOpenedNotebook();
    void save() const;
    bool isModified() const;
    bool isValid() const;

  private:
    mutable bool m_isValid = true;
    bool m_isModified = false;
    QJsonObject m_configObject;
    std::unordered_map<QString, QString> m_notebookMap;
    QString m_configFilePath;
    QString m_lastOpenedNotebook;
    void reportParseError(const QJsonParseError& parseError) const;
    void reportParseError(QStringView errorString) const;
};

#endif /* end of include gurd: __CONFIG_H__ */
