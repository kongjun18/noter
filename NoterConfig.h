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
 * @brief Call function depending on configuration
 *
 * If configuration field `field` with type `type` is not null, pass the const
 * lvalue copy of the value of `field` to `callable` and call it.
 *
 * @parm type Type of key: Bool, Array, Double, Int, String, QVariant
 * @parm field field of configuration
 * @parm callable any callable object, such as function, lambda
 * @note field need not be surround by quotation marks
 ******************************************************************************/
#define CONFIG_EDITOR(type, field, callable)                                   \
    do {                                                                       \
        if (auto iter{ editorConfig.find(QStringLiteral(#field)) };            \
            iter != editorConfig.end()) {                                      \
            if (auto variant{ iter->second }; !variant.isNull()) {             \
                const auto value{ variant.to##type() };                        \
                qDebug() << QString(#field " is %1").arg(value);               \
                callable(value);                                               \
            }                                                                  \
        }                                                                      \
    } while (0)

/*******************************************************************************
 * @brief Parse editor configuration
 *
 * This macro parses editor configuration, and insert {key, value} into
 * m_editorConfig.
 *
 * @parm type Type of key: Bool, Array, Double, Int, String, QVariant
 * @parm key Key of json object
 * @note field need not be surround by quotation marks
 ******************************************************************************/
#define PARSE_EDITOR_CONFIG(type, key)                                         \
    do {                                                                       \
        if (m_configObject.contains(QStringLiteral(#key))) {                   \
            m_editorConfig.insert(                                             \
              { QStringLiteral(#key),                                          \
                QVariant(m_configObject.value(#key).to##type()) });            \
        }                                                                      \
    } while (0)

/*******************************************************************************
 * @brief NoterConfig is designed to read, parse and process configuration file
 *
 * NoterConfig perhaps is not valid, so caller must ensure it is valid before
 * using it. NoterConfig is not valid only when configuration file is erroneous.
 * If configuration is empty, NoterConfig is valid.
 *
 * If NoterConfig is not valid or configuration is empty, all getter return
 *empty value.
 ******************************************************************************/
class NoterConfig
{
  public:
    NoterConfig();
    QString getNotebookPath(const QString& notebook) const;
    QStringList getNotebooks() const;
    QString getLastOpenedNotebook() const;
    QString getLanguage() const;
    const std::unordered_map<QString, QVariant>& getEditorConfig() const;
    QJsonObject::iterator setObject(const QString& key,
                                    const QJsonValue& value);
    QJsonObject::iterator addNotebook(const QString& notebook,
                                      const QString& path);
    QJsonObject::iterator setLastOpenedNotebook(
      const QString& lastOpenedNotebook);
    void removeNotebook(const QString& notebook);
    void removeLastOpenedNotebook();
    void save() const;
    bool isModified() const;
    bool isValid() const;

  private:
    mutable bool m_isValid = true;
    bool m_isModified = false;
    QString m_language;
    std::unordered_map<QString, QVariant> m_editorConfig;
    QJsonObject m_configObject;
    std::unordered_map<QString, QString> m_notebookMap;
    QString m_configFilePath;
    QString m_lastOpenedNotebook;
    void reportParseError(const QJsonParseError& parseError) const;
    void reportParseError(QStringView errorString) const;
};

#endif /* end of include gurd: __CONFIG_H__ */
