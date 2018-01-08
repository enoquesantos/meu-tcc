#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QDir>
#include <QObject>
#include <QVariant>

class App;
class PluginDatabaseTableCreator;

/**
 * @brief The PluginManager class has follow responsibilities:
 * 1: Manage the application plugins available from plugins directory. The plugins directory is specific from each platform:
 *  in Linux desktop, the plugins will be placed in appplication executable dir;
 *  in android, will be placed in assets:/plugins/;
 *  in ios/osx will be placed in assets_catalogs:/plugins/.
 * 2: Handle the plugins database table criation, if any of the plugins has a 'plugin_table.sql' file in your directory.
 * 3: Handle all plugins and application files cache deletion, when application are updated.
 * 4: When load all plugins, save in local settings the plugins pages list, to be used by follow QML components:
 *      Navigation Drawer menu retrieve the 'pages' list to show the options to the user;
 *      When app uses TabBar (specify in config.json), the 'pages' will be used to show the tab buttons to user navigate in the app.
 * 5: The Login and Home pages will be defined by some plugin, and will be saved in local settings to application window load in PageStack or SwipeView.
 * 5: Save the app version, used to decide when delete and reload all plugins and to optimize the app start'up.
 *      If app not updated (the version is the same of previous execution), abort and the object will be deleted by App instance (the parent of PluginManager).
 *      Otherwise, load all plugins and save in local settings.
 */
class PluginManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief PluginManager
     * The object constructor
     * @param parent *QObject a pointer to the parent object
     */
    explicit PluginManager(QObject *parent = nullptr);

    /**
     * @brief pluginsDir
     * Return the application plugins absolute directory path. The plugins directory is specific from each platform:
     * in Linux desktop, the plugins will be placed in appplication executable dir;
     * in android, will be placed in assets:/plugins/;
     * in ios/osx will be placed in assets_catalogs:/plugins/.
     * @return QDir
     */
    static QDir pluginsDir();

    /**
     * @brief setApp
     * Set a pointer to App object. Used to get access to qsettings methods.
     * The pointer will be set to m_app, the parent of this object.
     * @param app App*
     */
    void setApp(App *app);

    /**
     * @brief loadPlugins
     * Load all application plugins and apply some rules:
     * 1: Before load all plugins, check the app version with saved version (from previous execution).
     *    If saved version exists and is different, all qml cached will be deleted. Otherwise,
     *    if all plugins is already saved from old execution and app is not updated, abort and object will be deleted.
     * 2: For each plugin, will be check if a file with name 'plugin_table.sql' exists in your directory.
     *    If the file exists, the path will be pass to a new instance of PluginDatabaseTableCreator
     *    to execute the plugin table criation or migration (some update with new columns, for example.).
     * 3: For each plugin, load the config.json to read the pages and listeners.
     * 4: Pages needs to be an object array, and each page contains follow page properties:
     *    {
     *      "title": "the page title",
     *      "qml": "the main page file",
     *      "icon": "The page icon",
     *      "roles": ["a list of roles needed to access the page"],
     *      "order": 10,
     *      "isHome": false,
     *      "isLogin": true,
     *      "showInDrawer": true,
     *      "showInTabBar": false
     *    }
     *    The options booleans and integers options is optional and will be set to false or 0 (zero) if not is set.
     * After read all plugins, a pages array will be saved in local settings, and qml window or some specific object
     * like Menu.qml can read the array to create the pages options to user navigate in application.
     */
    void loadPlugins();

    /**
     * @brief sortByKey
     * This method is used to compare the 'order' value for each page.
     * The order is used to sort pages where showing into navigation drawer menu.
     * @param a QVariant a reference for plugin json object
     * @param b QVariant a reference for another plugin json object
     * @return bool
     */
    static bool sortByKey(const QVariant &a, const QVariant &b);

signals:
    /**
     * @brief finished
     * This signal is emitted when a intance of this class finished the job, sending
     * a signal to parent delete the pointer to improve the memory consumption.
     * @param thiz PluginManager* a pointer to this current instance
     */
    void finished(PluginManager *thiz);

private:
    /**
     * @brief save
     * This method save all object loaded data:
     *  1: the plugins pages array (after read and sort),
     *  2: the application version and,
     *  3: all listeners as string list
     */
    void save();

    /**
     * @brief clearCache
     * Remove all qml cached files, from application writable cache directory when application are updated.
     * The QmlEngine create a binary cache for each qml file. So, when the app are updated, we need to delete
     * all qml cached files to force the application to load the new changes for each component.
     * This trick uses the application version to decide if (or not) clear the cache.
     */
    void clearCache();

    /**
     * @brief createDatabaseTables
     * Search for 'plugin_table.sql' file in 'pluginDirPath'.
     * If a file with name 'plugin_table.sql' exists, will be loaded and pass to execute as sql query
     * in Database object and the table (or any sql query in the file) will be executed.
     * @param pluginDirPath QString a string with some plugin directory path
     */
    void createDatabaseTables(const QString &pluginDirPath);

    /**
     * @brief sortPages
     * Sort qml pages using the 'order' property value for all plugins pages.
     * This feature is needed to application show the pages sorted in navigation drawer menu or TabBar.
     * If the property 'order' is not set, the 0 (zero) value will be used.
     */
    void sortPages();

    /**
     * @brief parsePages
     * Check the values for each plugin page. The properties checked is:
     *  1: 'order' - used to sort pages. If no set, a 0 (zero) value will be used;
     *  2: 'showInDrawer' - used to show page in NavigationDrawer menu, when app uses drawer;
     *  4: 'showInTabBar' - used to show page in TabBar when app uses TabBar (set in config.json);
     *  5: 'roles' - a list of user roles, required to display (or not) to current user role;
     *  6: 'absPath' - the page absolute path used to Drawer Menu push in StackView after user click in some item;
     * @param pluginPath QString the pugin absolute path
     * @param pluginConfig QVariantMap a object from some plugin (the plugin config.json content)
     */
    void parsePages(const QString &pluginPath, const QVariantMap &pluginConfig);

private:
    /**
     * @brief m_pages
     * Pages array is used to keep the application plugins as a json array.
     * The pages is main feature of the app, and list the navigation options in NavigationDrawer menu (when is used)
     * or TabBar (put in window footer) when is used.
     */
    QVariantList m_pages;

    /**
     * @brief m_listeners
     * Keeps a list of all listeners, is added in 'loadPlugins' method and is used
     * by save() method. At end, save in local settings as QStringList
     */
    QStringList m_listeners;

    /**
     * @brief m_pluginsListPaths
     * Keeps a list of all plugins paths, is added in 'loadPlugins' method and is used
     * by plugins to retrieve the abolute path for earch plugin and can be used by plugin
     * to call a plugin file or page using the Config.plugins.pluginName -> get the absolute path to pluginName.
     */
    QVariantMap m_pluginsPaths;

    /**
     * @brief m_app
     * Keeps a pointer to this class instance parent and is set by void::setApp(App*) method.
     * This pointer is needed to access, ready and write in application settings (QSettings)
     */
    App *m_app;
};

#endif // PLUGINMANAGER_H
