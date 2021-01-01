#include "NoterConfig.h"

/*******************************************************************************
 * Constructor
 *
 * `NoterConfig` only support Windows and GNU/Linux. On Windows, configuration
 * file resides on current user's `AppData/Local/noter`. On GNU/Linux,
 * configuration file resieds on '~/.config/noter'.
 *
 * If configuration file don't exist, Constructor will create it.
 *
 * If configuration file is not valid, `isValid()` return false and all getter()
 * return empty value. Caller should handle return value perproly.
 *
 * @todo
 * - add support for windows
 * - add more options
 ******************************************************************************/
NoterConfig::NoterConfig()
{
    qDebug() << QStringLiteral("NoterConfig::NoterConfig(): in NoterConfig()");
#if defined(Q_OS_UNIX)
    QDir(QDir::homePath().append("/.config")).mkdir(QStringLiteral("noter"));
    m_configFilePath = QDir::homePath().append("/.config/noter/config.json");
#elif defined(Q_OS_WIN)
    QDir(QDir::homePath().append("/AppData/Local"))
      .mkdir(QStringLiteral("noter"));
    m_configFilePath =
      QDir::homePath().append(("/AppData/Local/noter/config.json"));
#endif
    QFile configFile(m_configFilePath);
    qDebug() << "m_configFilePath: " << m_configFilePath;

    // If configuration file don't exist, create it.
    if (!configFile.exists()) {
        if (!configFile.open(QIODevice::WriteOnly)) {
            reportParseError(u"Configuration don't exist and can't open it");
        }
        QTextStream configFileStream(&configFile);
        // default configuration
        configFileStream << QStringLiteral(R"(
{
    "centerOnScroll" : true, 
    "cursorLine": true,
    "fakeVim": true,
    "font": "",
    "fontSize" : 16,
    "lineWrap": true,
    "wordWrapMode" : "WrapAtWordBoundaryOrAnywhere",
    "autoSave": false,
    "tabStop": 4,
    "lastOpenedNotebook": "",
    "notebooks": {}
}
            )");
        if (configFileStream.pos() == -1) {
            QMessageBox::warning(
              nullptr,
              QObject::tr("Failed to initilize default configuration"),
              QObject::tr(R"(
Failed to write default configuration in file %1.
Please check configuration file.)")
                .arg(m_configFilePath));
        }
        return;
    }

    if (!configFile.open(QIODevice::ReadOnly)) {
        reportParseError(u"Can't open configuration");
        return;
    }
    // Construct json
    QJsonParseError parseError;
    const auto json{ configFile.readAll() };

    /*
     * If configuration file is empty, QJson can't parse it.
     * So just exit constructer and set`m_isValid` to false.
     */
    if (json.isEmpty()) {
        return;
    }

    // Parse configuration file
    QJsonDocument configDocument{ QJsonDocument::fromJson(json, &parseError) };
    if (parseError.error != QJsonParseError::NoError) {
        reportParseError(parseError);
        return;
    }
    if (!configDocument.isObject()) {
        reportParseError(QObject::tr("File don't contain object"));
        return;
    }

    // Parse field notebooks
    m_configObject = configDocument.object();
    if (!m_configObject.contains("notebooks")) {
        qDebug() << QStringLiteral(
          "NoterConfig::NoterConfig(): notebooks not found");
        m_isValid = false;
    } else {
        const auto value{ m_configObject.value("notebooks") };
        if (!value.isObject()) {
            reportParseError(
              QObject::tr("Field %1 in configuration is not object")
                .arg("value"));
            return;
        }
        const auto notebooks{ value.toObject() };
        for (const auto& key : notebooks.keys()) {
            const auto value{ notebooks.value(key) };
            if (value.isUndefined()) {
                reportParseError(
                  QObject::tr("Key %1 in object %2 don't has value")
                    .arg(key)
                    .arg("notebooks"));
                return;
            }
            qDebug() << "insert into  m_notebookMap "
                     << m_notebookMap.try_emplace(key, value.toString()).second;
        }
    }

    // Parse field lastOpenedNotebook
    if (m_configObject.contains("lastOpenedNotebook")) {
        m_lastOpenedNotebook =
          m_configObject.value("lastOpenedNotebook").toString();
    }

    // Parse editor configuration
    PARSE_EDITOR_CONFIG(Bool, cursorLine);
    PARSE_EDITOR_CONFIG(Bool, fakeVim);
    PARSE_EDITOR_CONFIG(Bool, centerOnScroll);
    PARSE_EDITOR_CONFIG(String, font);
    PARSE_EDITOR_CONFIG(Int, fontSize);
    PARSE_EDITOR_CONFIG(Bool, lineWrap);
    PARSE_EDITOR_CONFIG(String, wordWrapMode);
    PARSE_EDITOR_CONFIG(Bool, autoSave);
    PARSE_EDITOR_CONFIG(Int, tabStop);
}

/*******************************************************************************
 * @brief Report error of parsing
 *
 * Set `m_isValid` to false and report detailed message in standard warnning
 * messagebox.
 *
 * @param parseError QJsonParseError parser encoutered.
 ******************************************************************************/
void
NoterConfig::reportParseError(const QJsonParseError& parseError) const
{
    reportParseError(parseError.errorString());
}

/*******************************************************************************
 * @brief Report error of parsing
 *
 * Set `m_isValid` to false and report detailed message in standard warnning
 * messagebox.
 *
 * @param errorString Message you want to print
 ******************************************************************************/
void
NoterConfig::reportParseError(QStringView errorString) const
{
    m_isValid = false;
    QMessageBox::warning(
      nullptr,
      QObject::tr("Error in configuration file %1").arg(m_configFilePath),
      QObject::tr("Reason: %1").arg(errorString));
}

/*******************************************************************************
 * @brief Get path corresponding with `notebook`
 *
 * @param notebook Notebook name
 * @return Return notebook directory path if it exists, otherwise return empty
 * QString.
 ******************************************************************************/
QString
NoterConfig::getNotebookPath(const QString& notebook) const
{
    qDebug() << QString("argument notebook is %1").arg(notebook);
    qDebug() << QString("m_isValid is %1").arg(m_isValid);
    if (m_notebookMap.find(notebook) != m_notebookMap.end()) {
        return m_notebookMap.at(notebook);
    }
    return QString();
}

/*******************************************************************************
 * @brief Get notebook list
 ******************************************************************************/
QStringList
NoterConfig::getNotebooks() const
{
    QStringList notebookList;
    for (const auto pair : m_notebookMap) {
        notebookList.append(pair.first);
    }
    return notebookList;
}

/*******************************************************************************
 * @brief Return true if config is valid, otherwise return false.
 *
 * NoterConfig is not valid only when configuration file is erroneous.
 * If configuration is empty, NoterConfig is valid.
 *
 ******************************************************************************/
bool
NoterConfig::isValid() const
{
    return m_isValid;
}

/*******************************************************************************
 * @brief Return last opened notebook
 ******************************************************************************/
QString
NoterConfig::getLastOpenedNotebook() const
{
    return m_lastOpenedNotebook;
}

/*******************************************************************************
 * @brief Return true is config is modifed, otherwise return false
 ******************************************************************************/
bool
NoterConfig::isModified() const
{
    return m_isModified;
}

/*******************************************************************************
 * @brief Set configuration filed lastOpenedNotebook
 * @return An iterator pointing to the object
 * @see getLastOpenedNotebook()
 ******************************************************************************/
QJsonObject::iterator
NoterConfig::setLastOpenedNotebook(const QString& lastOpenedNotebook)
{
    return setObject(QStringLiteral("lastOpenedNotebook"),
                     QJsonValue(lastOpenedNotebook));
}

/*******************************************************************************
 * @brief set object with key 'key' to 'value' with type `QJsonValue`
 *
 * If object exists, remove it and add a new object {'key', 'value'}.
 * If object not exists, add a new object {'key', 'value'}.
 *
 * @param key Key of  object to be set
 * @param value Value of object to be set
 * @return An iterator pointing to the object
 ******************************************************************************/
QJsonObject::iterator
NoterConfig::setObject(const QString& key, const QJsonValue& value)
{
    if (m_configObject.contains(key)) {
        m_configObject.remove(key);
    }
    m_isModified = true;
    return m_configObject.insert(key, value);
}

/*******************************************************************************
 * @brief write NoterConfig object to configuration file
 *
 * For performance, if NoterConfig is not modifed, this function will not write
 *it.
 *
 * Configuration file(json) use indentted format.
 ******************************************************************************/
void
NoterConfig::save() const
{
    if (m_isModified) {
        QFile configFile(m_configFilePath);
        if (!configFile.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(
              nullptr,
              QObject::tr("Failed to open configuration"),
              QObject::tr("Please check your configuration file"));
            qCritical() << QString("Failed to open %1").arg(m_configFilePath);
        }
        QJsonDocument json{ m_configObject };
        configFile.write(json.toJson(QJsonDocument::Indented));
    }
}

/*******************************************************************************
 * @brief Add item {`notebook`, `path`} to object `notebooks`.
 *
 * @param notebook Name of notebook
 * @param path Path of notebook
 * @return Iterator pointing to next item(same level with `notebooks`)
 ******************************************************************************/
QJsonObject::iterator
NoterConfig::addNotebook(const QString& notebook, const QString& path)
{
    QJsonObject newObject;
    if (m_configObject.contains("notebooks")) {
        newObject = m_configObject.value("notebooks").toObject();
    }
    newObject.insert(notebook, QJsonValue(path));
    m_notebookMap.insert(std::make_pair(notebook, path));
    return setObject("notebooks", QJsonValue(newObject));
}

/*******************************************************************************
 * @brief Remove notebook in `notebooks`
 ******************************************************************************/
void
NoterConfig::removeNotebook(const QString& notebook)
{
    if (!m_configObject.contains("notebooks")) {
        return;
    }
    auto newObject{ m_configObject.value("notebooks").toObject() };
    newObject.remove(notebook);
    m_notebookMap.erase(notebook);
    setObject("notebooks", QJsonValue(newObject));
}

void
NoterConfig::removeLastOpenedNotebook()
{
    m_configObject.remove(QStringLiteral("lastOpenedNotebook"));
}

/*******************************************************************************
 * @brief Return an unordered map stores configuration related to editor
 ******************************************************************************/
std::unordered_map<QString, QVariant>
NoterConfig::getEditorConfig() const
{
    return m_editorConfig;
}
